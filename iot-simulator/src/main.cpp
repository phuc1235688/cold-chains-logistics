#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include "../../backend-core/domain/iot/ISensorReadingHandler.h"
#include "../../backend-core/domain/iot/SensorDevice.h"
#include "../include/SensorGateway.h"
#include "../include/SensorSimulator.h"

// Mock Handler đại diện cho Phân hệ Shipment của TV1
class ShipmentReadingReceiver : public Domain::IoT::ISensorReadingHandler {
private:
    int violationCount = 0;
    const double MAX_THRESHOLD = 8.0;
    const int CRITICAL_LIMIT = 3;

public:
    void onReading(const Domain::IoT::TemperatureReading& reading) override {
        std::cout << "  -> Nhan du lieu: Sensor = " << reading.getSensorId()
                  << " | Nhiet do = " << reading.getCelsius() << " C"
                  << " | Thoi gian = " << reading.getTimestamp() << "\n";

        // Nghiệp vụ TV1 kiểm tra điều kiện bảo quản chuỗi lạnh
        if (reading.getCelsius() > MAX_THRESHOLD) {
            violationCount++;
            std::cout << "     [! CANH BAO] Nhiet do vuot nguong an toan (" 
                      << MAX_THRESHOLD << " C)! So lan vi pham lien tiep: " << violationCount << "\n";

            if (violationCount >= CRITICAL_LIMIT) {
                std::cout << "     [!!! NGUY HIEM] VI PHAM QUA " << CRITICAL_LIMIT 
                          << " LAN -> KICH HOAT LENH HUY LO HANG TRONG CONTAINER!\n";
            }
        } else {
            if (violationCount > 0) {
                std::cout << "     [✓ HOI PHUC] Nhiet do da tro ve dải an toan. Reset bo dem vi pham.\n";
                violationCount = 0;
            }
        }
    }
};

int main() {
    std::cout << "======================================================================\n";
    std::cout << "   HE THONG CHUOI CUNG UNG LANH (COLD CHAIN LOGISTICS)   \n";
    std::cout << "======================================================================\n\n";

    // 1. Khởi tạo Domain Entity SensorDevice
    Domain::IoT::SensorDevice containerSensor("SENSOR-COLD-01", "CONTAINER-HCM-01");
    std::cout << "[Domain] Khoi tao SensorDevice Entity:\n";
    std::cout << "  - Sensor ID   : " << containerSensor.getSensorId() << "\n";
    std::cout << "  - Container ID: " << containerSensor.getContainerId() << "\n";
    std::cout << "  - Trang thai  : " << containerSensor.getStatusString() << "\n\n";

    // 2. Khởi tạo Gateway (Anti-Corruption Layer) và đăng ký Handler TV1
    auto gateway = std::make_shared<SensorGateway>();
    auto tv1Handler = std::make_shared<ShipmentReadingReceiver>();
    gateway->registerHandler(tv1Handler);
    gateway->registerDevice(containerSensor.getSensorId(), containerSensor.getContainerId());
    std::cout << "[Gateway-ACL] Da dang ky SensorGateway va ket noi ISensorReadingHandler.\n\n";

    // 3. Khởi tạo Simulator (chu kỳ 1000ms)
    auto simulator = std::make_unique<SensorSimulator>(gateway, 1000);
    simulator->addSensor(containerSensor.getSensorId());

    // 4. Menu lựa chọn kịch bản
    std::cout << "CHON KICH BAN MO PHONG CAM BIEN:\n";
    std::cout << "  1. STABLE: On dinh trong dai an toan (2.5C - 4.8C)\n";
    std::cout << "  2. DRIFT : Hong thung / That thoat nhiet (Nhiet do tang dan deu)\n";
    std::cout << "  3. SPIKE : Dot bien nhiet tuc thoi (Vuot nguong warning roi tu hoi phuc)\n";
    std::cout << "  4. NOISE : Nhieu ngau nhien bien do rong (Cac dao dong nhiet bat thuong)\n";
    std::cout << "Nhap lua chon (1-4) [Mac dinh 1]: ";

    int choice = 1;
    if (!(std::cin >> choice)) {
        choice = 1;
    }

    switch (choice) {
        case 2:
            simulator->setMode(SimulationMode::DRIFT);
            break;
        case 3:
            simulator->setMode(SimulationMode::SPIKE);
            break;
        case 4:
            simulator->setMode(SimulationMode::NOISE);
            break;
        default:
            simulator->setMode(SimulationMode::STABLE);
            break;
    }

    std::cout << "\n--> Che do dang chay: " << simulator->getModeString() << "\n";
    std::cout << "--> Bat dau mo phong (Du lieu duoc gui moi 1 giay trong 5 chu ky)\n";

    simulator->start();

    // Chờ người dùng nhấn Enter để dừng
    std::cin.ignore(10000, '\n');
    std::cin.get();

    std::cout << "\n[Simulator] Dang dung luong an toan (workerThread.join())...\n";
    simulator->stop();
    std::cout << "[Simulator] Da dung luong thanh cong.\n";

    // Thử nghiệm kiểm tra Timeout OFFLINE sau khi dừng
    std::cout << "\n[Heartbeat Check] Kiem tra timeout mat ket noi sau khi ngung phat du lieu:\n";
    std::cout << "  Trang thai hien tai: " << gateway->getDeviceStatusString(containerSensor.getSensorId()) << "\n";
    std::cout << "  Gia lap he thong kiem tra sau 3 giay khong co tin hieu...\n";
    gateway->checkDeviceTimeouts(0); // Timeout ngay vì luồng đã dừng
    std::cout << "  Trang thai sau kiem tra: " << gateway->getDeviceStatusString(containerSensor.getSensorId()) << "\n";

    std::cout << "\n=== CHUONG TRINH HOAN TAT AN TOAN ===\n";
    return 0;
}
