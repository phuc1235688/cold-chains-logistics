cold-chain-logistics/

├── iot-simulator/         # Layer 1 — điểm mở rộng 

├── iot-gateway/           # Layer 2 — điểm mở rộng 

├── backend-core/          # Layer 3 — Domain (DDD) + Observer + State + Strategy

│   ├── domain/

│   │   ├── entities/      # LoHang (Aggregate Root), ThungLanh, CamBienIoT, LoTrinh

│   │   ├── value_objects/ # KhoangNhietDo, DuLieuNhietDo, DiemLoTrinh

│   │   ├── state/         # State Pattern: ITrangThaiLoHang + 4 trạng thái cụ thể

│   │   └── strategy/      # Strategy Pattern: IChienLuocNguongNhietDo + 3 chiến lược

│   ├── observer/          # Observer Pattern: NguoiQuanSat, DichVuCanhBao

│   ├── application/       # MoPhongIoT (mô phỏng dữ liệu IoT trong tiến trình)

│   ├── repository/        # Repository Pattern: ILoHangRepository + bản in-memory

│   ├── util/               # Hàm nhập liệu an toàn (NhapLieu)

│   └── main.cpp            # Điểm vào chương trình

├── frontend-dashboard/     # Layer 4 — Console UI (ConsoleUI.h/.cpp)

├── docs/                   # Tài liệu: use-case, class diagram, sequence diagram...

├── tests/                  # (chưa có test tự động — xem README)

├── config/                 # File cấu hình mẫu

├── CMakeLists.txt

└── README.md

Trong đó:

iot-gateway/

├── include/

│   ├── IoTGateway.h

│   ├── DataValidator.h

│   ├── DataPacket.h

│   └── CloudClient.h

├── src/

│   ├── IoTGateway.cpp

│   ├── DataValidator.cpp

│   ├── DataPacket.cpp

│   └── CloudClient.cpp

├── config/

│   └── gateway_config.json

├── main.cpp

└── CMakeLists.txt

iot-simulator/

├── include/

│   ├── Sensor.h

│   ├── TemperatureSensor.h

│   ├── HumiditySensor.h

│   └── SensorData.h

├── src/

│   ├── Sensor.cpp

│   ├── TemperatureSensor.cpp

│   ├── HumiditySensor.cpp

│   └── SensorData.cpp

├── config/

│   └── sensor_config.json

├── main.cpp

└── CMakeLists.txt
