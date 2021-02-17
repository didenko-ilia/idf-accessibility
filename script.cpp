#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include <jansson.h>
#include <curl/curl.h>

using namespace std;

string URL;

static char errorBuffer[CURL_ERROR_SIZE];

// source https://curl.se/libcurl/c/htmltitle.html
static int writer(char *data, size_t size, size_t nmemb,
                  std::string *writerData)
{
  if(writerData == NULL)
    return 0;
 
  writerData->append(data, size*nmemb);
 
  return size * nmemb;
}

static bool init(CURL *&conn, const char *url, std::string &buffer)
{
  CURLcode code;
 
  conn = curl_easy_init();

 
  if(conn == NULL) {
    fprintf(stderr, "Failed to create CURL connection\n");
    exit(EXIT_FAILURE);
  }
 
  code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set error buffer [%d]\n", code);
    return false;
  }
 
  code = curl_easy_setopt(conn, CURLOPT_URL, url);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
    return false;
  }
 
  code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
    return false;
  }
 
  code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
    return false;
  }
 
  code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
    return false;
  }
 
  return true;
}

static char *request(std::string &url, std::string &buffer) {
  CURL *curl = NULL;
  CURLcode status;

  curl_global_init(CURL_GLOBAL_ALL);

  if (!init(curl, &url[0], buffer)) {
    fprintf(stderr, "Error: curl initialization failed");
  }

  status = curl_easy_perform(curl);

  if(status != CURLE_OK) {
    fprintf(stderr, "Failed to get '%s' [%s]\n", &url[0], errorBuffer);
    exit(EXIT_FAILURE);
  }

  curl_easy_cleanup(curl);
}

long long parseTravelTime(std::string &jsonString) {
  json_t *root;
  json_error_t error;

  root = json_loads(&jsonString[0], 0, &error);

  if (!root) {
    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    return 1;
  }

  json_t *plan = json_object_get(root, "plan");
  json_t *itineraries = json_object_get(plan, "itineraries");
  json_t *bestItinerarie = json_array_get(itineraries, 0);
  json_t *duration = json_object_get(bestItinerarie, "duration");
  return json_integer_value(duration);
}

string queryAtoB(string coordA, string coordB, string ip) {
  string buffer;
  string url = "http://" + ip + "/otp/routers/default/plan?";
  string fromCoord = coordA;
  url += "fromPlace=" + fromCoord;
  string toCoord = coordB;
  url += "&toPlace=" + toCoord;
  string date = "01-27-2021";
  url += "&date=" + date;
  string time = "1:00pm";
  url += "&time=" + time;
  string maxWalkDistance = "2500";
  url += "&maxWalkDistance=" + maxWalkDistance;
  url += "&numItineraries=1";
  request(url,buffer);
  return buffer;
}

int main(int argc, char* argv[])
{
  if (argc < 2) {
    cout << "Missing ip/port pair of the OTP instance. Usage : ./script <addr>\n";
    return 0;
  }

  string ip = argv[1];
	cout << "Connecting to the OTP server on "<< ip << "\n";

  ifstream fin;
  fin.open("laposte_hexasmal.csv");
  string s;
  vector<string> name;
  vector<string> coord;
  vector<string> code_insee;
  set<string> knownPostalCodes;
  while (getline(fin, s)) {
    // split, take name, code and coords
    vector<string> values;
    int prev = 0, pos = 0;
    do {
      pos = s.find(';', prev);
      if (pos == string::npos) pos = s.length();
      string value = s.substr(prev, pos - prev);
      values.push_back(value);
      prev = pos + 1;
    } while (pos < s.length() && prev < s.length());
    string depCode = values[2].substr(0,2);
    // the original file has \r\n end symbol since it was written on Windows
    string coordClean = values[5].substr(0,values[5].size() - 1); 
    if ((depCode == "94" || depCode == "93" || depCode == "92" || depCode == "75")
          &&(knownPostalCodes.find(values[2])==knownPostalCodes.end())) {
      name.push_back(values[1]);
      coord.push_back(coordClean);
      code_insee.push_back(values[0]);
      knownPostalCodes.insert(values[2]);
    }
  }
  fin.close();

  vector<double> accesibility;
  int N = name.size();
  for (int i = 0; i<N; ++i) {
    int total = 0;
    for (int j = 0; j<N; ++j) {
      if (i != j) {
        string queryJSON = queryAtoB(coord[j], coord[i], ip);
        total += parseTravelTime(queryJSON);
      }
    }
    double avg = 1.0*total / (N - 1);
    accesibility.push_back(avg);
    cout << name[i] << ' ' << coord[i] << ' ' << avg << '\n';
  }

  ofstream fout;
  fout.open("access_petite_couronne.csv");
  fout << "Code_INSEE;Name;Coord;Access\n";
  for (int i = 0; i<N; ++i) {
    fout << code_insee[i] << ';' << name[i] << ';' << coord[i] << ';' << accesibility[i] << '\n';
  }
  fout.close();
}
