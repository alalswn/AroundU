#include <iostream>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <windows.h> // For SetConsoleOutputCP

// nlohmann::json 라이브러리를 위한 별칭
using json = nlohmann::json;

// --- 1. API 호출을 담당하는 클래스 ---
// 이 클래스의 유일한 책임은 cURL을 사용하여
// 지정된 URL의 데이터를 가져와 문자열로 반환하는 것입니다.
class ApiClient {
public:
    // 생성자: cURL 전역 환경 초기화 (RAII 패턴)
    ApiClient() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    // 소멸자: cURL 전역 환경 정리 (RAII 패턴)
    ~ApiClient() {
        curl_global_cleanup();
    }

    // fetchData: 지정된 URL에서 데이터를 가져와 문자열로 반환
    // 성공 시: 데이터 문자열 반환
    // 실패 시: 빈 문자열 반환
    std::string fetchData(const std::string& url) {
        CURL* curl;
        CURLcode res;
        std::string response_data; // 데이터를 저장할 로컬 변수

        // cURL 세션 초기화
        curl = curl_easy_init();
        if (curl) {
            // 1. 요청할 URL 설정
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            
            // 2. 응답 데이터를 처리할 *static* 콜백 함수 설정
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ApiClient::WriteCallback);
            
            // 3. 콜백 함수로 전달할 사용자 데이터 (response_data 문자열 포인터) 설정
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

            // 4. HTTP 요청 실행
            res = curl_easy_perform(curl);

            // 5. 오류 검사
            if (res != CURLE_OK) {
                std::cerr << "cURL 요청 실패: " << curl_easy_strerror(res) << std::endl;
                response_data = ""; // 실패 시 빈 문자열로 초기화
            }

            // cURL 세션 정리
            curl_easy_cleanup(curl);
        } else {
            std::cerr << "cURL 초기화 실패" << std::endl;
            response_data = ""; // 실패 시 빈 문자열로 초기화
        }

        // 성공 시 수신된 데이터, 실패 시 빈 문자열 반환
        return response_data;
    }

private:
    // --- cURL이 수신한 데이터를 저장하기 위한 *static* 콜백 함수 ---
    // cURL은 C 라이브러리이므로, C++ 클래스의 non-static 멤버 함수를
    // 직접 콜백으로 사용할 수 없습니다. (this 포인터 문제)
    // 따라서 반드시 static 함수로 선언해야 합니다.
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t realsize = size * nmemb;
        ((std::string*)userp)->append((char*)contents, realsize);
        return realsize;
    }
};

// --- 2. 프로그램을 실행하는 메인 함수 ---
int main() {
    // === 콘솔 인코딩 설정 ===
    SetConsoleOutputCP(CP_UTF8);
    
    // API URL 정의
    const std::string API_URL = "http://openapi.seoul.go.kr:8088/4971614f7663686f38307a777a6849/json/culturalEventInfo/1/5/";

    // 1. ApiClient 객체 생성
    ApiClient client;

    // 2. API 호출하여 데이터 가져오기 (이제 한 줄로 가능!)
    std::string raw_data = client.fetchData(API_URL);

    // 3. 데이터 수신 성공 여부 확인
    if (raw_data.empty()) {
        std::cerr << "API로부터 데이터를 가져오는데 실패했습니다." << std::endl;
        return 1; // 프로그램 비정상 종료
    }

    // --- 4. 데이터 파싱 및 출력 (이 부분이 조원들의 클래스로 대체될 부분) ---
    // 이제 main 함수(또는 다른 클래스)가 데이터 처리/출력을 담당합니다.
    std::cout << "--- API 요청 성공 ---" << std::endl;
    
    try {
        // nlohmann::json을 사용하여 문자열을 JSON 객체로 파싱
        json api_json = json::parse(raw_data);

        std::cout << "\n--- 서울 문화행사 데이터 추출 결과 ---" << std::endl;


        // 아래는 정상 호출 확인 여부를 위해 임의로 추가한 코드입니다.
        // 이곳을 기점으로 이하의 내용 - 145번째 줄까지 -을 지우고 각자 클래스에서 구현해주신 부분을 적용하면 될 것 같습니다!
      
        // 1. 최상위 객체 "culturalEventInfo"에 접근
        if (api_json.contains("culturalEventInfo")) {
            const json& event_info = api_json["culturalEventInfo"];

            // 2. 이벤트 목록 배열 "row"에 접근
            if (event_info.contains("row") && event_info["row"].is_array()) {
                const json& event_rows = event_info["row"];

                std::cout << "총 " << event_rows.size() << "건의 행사 정보를 가져왔습니다." << std::endl;
                std::cout << "------------------------------------" << std::endl;

                // 3. 배열을 순회하며 각 이벤트 정보 출력
                int count = 1;
                for (const auto& event : event_rows) {
                    std::string title = event.contains("TITLE") ? event["TITLE"].get<std::string>() : "제목 정보 없음";
                    std::string codename = event.contains("CODENAME") ? event["CODENAME"].get<std::string>() : "분류 정보 없음";
                    std::string date = event.contains("DATE") ? event["DATE"].get<std::string>() : "일시 정보 없음";

                    std::cout << "[" << count++ << "]" << std::endl;
                    std::cout << "  제목: " << title << std::endl;
                    std::cout << "  분류: " << codename << std::endl;
                    std::cout << "  일시: " << date << std::endl;
                    std::cout << "------------------------------------" << std::endl;
                }
            } else {
                std::cerr << "JSON 구조 오류: 'row' 배열을 찾을 수 없거나 데이터가 비어 있습니다." << std::endl;
            }

        } else {
            std::cerr << "JSON 구조 오류: 'culturalEventInfo' 객체를 찾을 수 없습니다." << std::endl;
            if (api_json.contains("RESULT")) {
                std::cerr << "API 응답 오류 메시지: " << api_json["RESULT"].dump() << std::endl;
            }
        }
    } catch (json::exception& e) {
        std::cerr << "\nJSON 파싱 오류: " << e.what() << std::endl;
    }

    return 0;
