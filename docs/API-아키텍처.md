# Unframed — API / 아키텍처 요약

> **SSOT: `docs/명세서/Unframed_API명세서.xlsx`** (README / Endpoints / Detail-대화퀴즈봉인 / Detail-도감세션 / Data Models / Error Codes).
> 이 문서는 그 엑셀(v0.1 초안)의 요약본이다. 충돌 시 **엑셀이 우선**. 필드 상세·예시 JSON은 엑셀 참고.

## 아키텍처 — (B) 백엔드 중계

```
언리얼(클라이언트) ──HTTP──> 백엔드 서버 ──> 생성형 AI API (LLM / TTS)
                               └── DB (도감·통계)
```

- 언리얼은 **백엔드만** 호출. AI 호출은 백엔드 내부에 숨김(아래 ★).
- 대화 맥락·통계를 서버 한곳에서 관리 → "대화 기반 퀴즈 생성"이 깔끔. API 키도 서버에만 → 안전.
- **Base URL**: `https://api.unframed.example` (배포 시 교체)
- **응답 형식**: 모든 응답 JSON. 성공 2xx / 클라 오류 4xx / 서버 오류 5xx.
- **인증**: MVP는 단순 `playerId` 기반(로그인 생략). 실서비스 시 토큰 인증으로 교체 가능.

## 엔드포인트 (8개)

| # | 기능 | Method | Endpoint | AI 중계 |
|---|---|---|---|---|
| 1 | 세션 시작 | POST | `/sessions` | |
| 2 | 명화 포획 | POST | `/captures` | |
| 3 | 명화 대화 | POST | `/paintings/{paintingId}/chat` | ★ |
| 4 | 퀴즈 생성 | POST | `/paintings/{paintingId}/quiz` | ★ |
| 5 | 봉인(정답 제출) | POST | `/paintings/{paintingId}/seal` | ★ |
| 6 | 도감 조회 | GET | `/players/{playerId}/collection` | |
| 7 | 진척/통계 조회 | GET | `/players/{playerId}/progress` | |
| 8 | 세션 종료 | POST | `/sessions/{sessionId}/complete` | |

★ = 백엔드가 AI(LLM/TTS)를 내부 호출. 언리얼은 차이를 모름(똑같이 백엔드만 호출).

게임 한 사이클: `/sessions`(1) → `/captures`(2) → `/chat`(3, 반복) → `quizReady` → `/quiz`(4) → `/seal`(5) → 3종 완료 시 `/complete`(8). 도감/통계(6,7)는 재방문·시연용.

## 데이터 모델 (Data Models 시트)

- **Painting**: `paintingId, title, artist, era, personaPrompt, assetKey` — 명화 추가 = 한 행 추가.
- **Session**: `sessionId, playerId, targets[], startedAt, playTimeSec`
- **SealRecord(도감 항목)**: `playerId, paintingId, sealedAt, attempts, quizAccuracy`
- 봉인 목표 3종: `["mona_lisa", "the_scream", "starry_night"]` (모나리자·뭉크·반 고흐).

## 에러 코드 (Error Codes 시트)

형식: `{ "error": { "code": ..., "message": ... } }`

`INVALID_REQUEST(400)`, `SESSION_NOT_FOUND(404)`, `PAINTING_NOT_FOUND(404)`, `ALREADY_SEALED(409)`,
`QUIZ_NOT_READY(422)`, `RATE_LIMITED(429)`, `AI_UPSTREAM_ERROR(500)`, `SERVICE_UNAVAILABLE(503)`.

## 로컬 폴백

- 각 API 응답과 **동일 모양**의 로컬 파일을 둔다(`docs/data/*.json`).
- 데이터 소스(서버 ↔ 로컬)는 스위치 하나로 전환 → 게임 로직 무변경. → `docs/데이터-계약서.md`

## TODO

- [ ] 실제 스택 확정: LLM API, TTS, DB 종류, 실제 Base URL, 인증 토큰 방식
- [ ] 엑셀을 정식 v1으로 승격(현재 v0.1 초안), 변경 시 엑셀 먼저 수정
