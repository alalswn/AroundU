
# Seoul Culture Event Recommender (Console-first)

콘솔 기반으로 시작하고, 이후 웹 레이어(FastAPI/Express 등)를 얹을 수 있도록 설계된 C++ 프로젝트 스타터입니다.

## Features (MVP 목표)
- 하버사인 거리 계산
- (추후) 서울시/카카오맵 API 연동
- (추후) 위치 반경 내 행사 추천

## Build
```bash
make
./program
```

## Run (예시)
```bash
# 거리 계산 테스트: lat1 lon1 lat2 lon2
./program 37.5665 126.9780 37.5700 126.9768
```

## Structure
```
seoul-event-recommender/
├─ src/
│  ├─ main.cpp
│  ├─ core/
│  │  ├─ geo.cpp
│  │  └─ recommender.cpp        (stub)
│  └─ api/
│     ├─ seoul_api.cpp          (stub)
│     └─ kakao_api.cpp          (stub)
├─ include/
│  ├─ geo.h
│  ├─ recommender.h             (stub)
│  ├─ seoul_api.h               (stub)
│  └─ kakao_api.h               (stub)
├─ third_party/
│  └─ README.md                 (header-only 라이브러리 안내)
├─ tests/
│  └─ test_geo.cpp              (간단 테스트)
├─ docs/
│  └─ requirements.md           (빈 문서; 팀이 채우세요)
├─ .gitignore
├─ Makefile
└─ README.md
```

## Third-party headers (추가 예정)
- HTTP: cpp-httplib (single Header)
- JSON: nlohmann/json (single Header)

필요할 때 `third_party/` 폴더에 헤더 파일을 추가하면 됩니다.
