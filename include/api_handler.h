#include <iostream>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
// #include <locale> // Locale 헤더는 더 이상 필요하지 않습니다.

// [추가] Windows API를 사용하여 콘솔 출력을 UTF-8로 강제 설정
#include <windows.h> 

// nlohmann::json 라이브러리를 위한 별칭
using json = nlohmann::json;

// --- 1. cURL이 수신한 데이터를 저장하기 위한 콜백 함수 ---
// (기존과 동일)
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // 실제 데이터 크기 계산
    size_t realsize = size * nmemb;
    // userp는 응답 데이터를 저장할 std::string 객체에 대한 포인터
    ((std::string*)userp)->append((char*)contents, realsize);
    return realsize;
}

// --- 2. API 요청 및 JSON 파싱을 수행하는 메인 함수 ---
int main() {
    // === [인코딩 문제 해결을 위한 코드 수정] ===
    // Windows API를 사용하여 이 프로그램이 실행되는 콘솔의
    // 출력 코드 페이지를 UTF-8 (65001)로 설정합니다.
    SetConsoleOutputCP(CP_UTF8);
    // ===========================================
    
    // API URL 정의 (수정됨: 서울시 문화행사 정보 API, JSON 형식으로 요청)
    // (인증키)/json/culturalEventInfo/1/5/ 구조를 사용하며, 인증키는 사용자님의 것을 적용했습니다.
    const std::string API_URL = "http://openapi.seoul.go.kr:8088/4971614f7663686f38307a777a6849/json/culturalEventInfo/1/5/";
    std::string response_data; // API 응답 데이터를 저장할 문자열

    // cURL 핸들러
    CURL* curl;
    CURLcode res;

    // cURL 전역 환경 초기화
    curl_global_init(CURL_GLOBAL_DEFAULT);
    // cURL 세션 초기화
    curl = curl_easy_init();

    if (curl) {
        // 1. 요청할 URL 설정
        curl_easy_setopt(curl, CURLOPT_URL, API_URL.c_str());

        // 2. 응답 데이터를 처리할 콜백 함수 설정
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // 3. 콜백 함수로 전달할 사용자 데이터 (response_data 문자열 포인터) 설정
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // 4. HTTP 요청 실행 및 결과 저장
        res = curl_easy_perform(curl);

        // 오류 검사
        if (res != CURLE_OK) {
            std::cerr << "cURL 요청 실패: " << curl_easy_strerror(res) << std::endl;
        } else {
            // 요청 성공
            std::cout << "--- API 요청 성공 ---" << std::endl;
            // 디버깅 목적으로 수신된 전체 JSON 데이터 출력 (선택 사항)
            // std::cout << "수신된 JSON 데이터:\n" << response_data.substr(0, 500) << "..." << std::endl; 

            // --- 5. JSON 데이터 파싱 및 출력 (서울시 API 구조에 맞게 수정됨) ---
            try {
                // nlohmann::json을 사용하여 문자열을 JSON 객체로 파싱
                json api_json = json::parse(response_data);

                std::cout << "\n--- 서울 문화행사 데이터 추출 결과 ---" << std::endl;

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
                            // API 명세에 따라 필드 이름은 대문자 (TITLE, CODENAME, DATE 등)
                            // .get<std::string>()을 사용하여 명시적으로 문자열로 추출
                            // API 응답 데이터도 한글이므로 출력 시 깨짐 현상이 발생할 수 있습니다.
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
                    // API 인증키 오류 등의 경우 'RESULT' 객체에 오류 메시지가 포함될 수 있음
                    if (api_json.contains("RESULT")) {
                        std::cerr << "API 응답 오류 메시지: " << api_json["RESULT"].dump() << std::endl;
                    }
                }

            } catch (json::exception& e) {
                // 파싱 오류 처리
                std::cerr << "\nJSON 파싱 오류: " << e.what() << std::endl;
            }
        }

        // cURL 세션 정리
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "cURL 초기화 실패" << std::endl;
    }

    // cURL 전역 환경 정리
    curl_global_cleanup();
    
    return 0;
}
