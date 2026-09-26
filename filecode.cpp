#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

// ======================================================
// HAM NHAP DU LIEU AN TOAN
// ======================================================

string nhapChuoi(const string& thongBao) {
    string duLieu;

    while (true) {
        cout << thongBao;
        getline(cin, duLieu);

        if (!duLieu.empty()) {
            return duLieu;
        }

        cout << "Khong duoc de trong. Vui long nhap lai.\n";
    }
}

int nhapSoNguyen(const string& thongBao, int minValue, int maxValue) {
    int giaTri;

    while (true) {
        cout << thongBao;

        if (cin >> giaTri && giaTri >= minValue && giaTri <= maxValue) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return giaTri;
        }

        cout << "Gia tri khong hop le. Vui long nhap lai.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double nhapSoThuc(const string& thongBao, double minValue, double maxValue) {
    double giaTri;

    while (true) {
        cout << thongBao;

        if (cin >> giaTri && giaTri >= minValue && giaTri <= maxValue) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return giaTri;
        }

        cout << "Gia tri khong hop le. Vui long nhap lai.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}


// ======================================================
// 1. VALUE OBJECT: KHOANG NHIET DO
// ======================================================

class KhoangNhietDo {
private:
    double toiThieu;
    double toiDa;

public:
    KhoangNhietDo(double min = 0.0, double max = 0.0) {
        toiThieu = min;
        toiDa = max;
    }

    double getToiThieu() const {
        return toiThieu;
    }

    double getToiDa() const {
        return toiDa;
    }

    bool hopLe(double nhietDo) const {
        return nhietDo >= toiThieu &&
               nhietDo <= toiDa;
    }
};


// ======================================================
// 2. VALUE OBJECT: DU LIEU NHIET DO
// ======================================================

class DuLieuNhietDo {
private:
    double nhietDo;
    double doAm;
    time_t thoiGian;

public:
    DuLieuNhietDo(double nhietDo, double doAm) {
        this->nhietDo = nhietDo;
        this->doAm = doAm;
        this->thoiGian = time(NULL);
    }

    double getNhietDo() const {
        return nhietDo;
    }

    double getDoAm() const {
        return doAm;
    }

    string getThoiGian() const {
        char buffer[80];
        tm* localTime = localtime(&thoiGian);

        if (localTime == NULL) {
            return "Khong xac dinh";
        }

        strftime(
            buffer,
            sizeof(buffer),
            "%d/%m/%Y %H:%M:%S",
            localTime
        );

        return string(buffer);
    }
};


// ======================================================
// 3. STRATEGY PATTERN: CHIEN LUOC NGUONG NHIET DO
// ======================================================
// Moi loai hang hoa (vac-xin, thuc pham tuoi song, thuc pham
// dong lanh...) co mot nguong nhiet do va cach danh gia rieng.
// Thay vi hard-code 1 khoang nhiet do chung cho moi LoHang,
// ta tach logic nay thanh cac Strategy doc lap, de dang mo
// rong them loai hang moi ma khong sua code cu (Open/Closed).
// ======================================================

class IChienLuocNguongNhietDo {
public:
    virtual ~IChienLuocNguongNhietDo() {}

    virtual KhoangNhietDo layKhoangNhietDo() const = 0;

    virtual bool vuotNguong(double nhietDo) const {
        return !layKhoangNhietDo().hopLe(nhietDo);
    }

    virtual string tenChienLuoc() const = 0;
};


class ChienLuocVacXin : public IChienLuocNguongNhietDo {
public:
    KhoangNhietDo layKhoangNhietDo() const override {
        // Theo khuyen nghi WHO cho phan lon vac-xin (ILR): 2-8 C
        return KhoangNhietDo(2.0, 8.0);
    }

    string tenChienLuoc() const override {
        return "Vac-xin (2 - 8 C, theo khuyen nghi WHO)";
    }

    static ChienLuocVacXin& layInstance() {
        static ChienLuocVacXin instance;
        return instance;
    }
};


class ChienLuocThucPhamTuoiSong : public IChienLuocNguongNhietDo {
public:
    KhoangNhietDo layKhoangNhietDo() const override {
        // Nhiet do bao quan lanh cho thuc pham tuoi/rau qua: 0-4 C
        return KhoangNhietDo(0.0, 4.0);
    }

    string tenChienLuoc() const override {
        return "Thuc pham tuoi song (0 - 4 C)";
    }

    static ChienLuocThucPhamTuoiSong& layInstance() {
        static ChienLuocThucPhamTuoiSong instance;
        return instance;
    }
};


class ChienLuocThucPhamDongLanh : public IChienLuocNguongNhietDo {
public:
    KhoangNhietDo layKhoangNhietDo() const override {
        // Theo quy dinh thuc pham dong lanh (vd: Phan Lan/EU): -25 den -18 C
        return KhoangNhietDo(-25.0, -18.0);
    }

    string tenChienLuoc() const override {
        return "Thuc pham dong lanh (-25 - -18 C)";
    }

    static ChienLuocThucPhamDongLanh& layInstance() {
        static ChienLuocThucPhamDongLanh instance;
        return instance;
    }
};


// ======================================================
// 4. OBSERVER INTERFACE
// ======================================================

class NguoiQuanSat {
public:
    virtual ~NguoiQuanSat() {}

    virtual void xuLyCanhBao(
        const string& maCamBien,
        const string& maThungLanh,
        const DuLieuNhietDo& duLieu,
        const KhoangNhietDo& khoang
    ) = 0;
};


// ======================================================
// 5. CAM BIEN IOT - SUBJECT
// ======================================================

class CamBienIoT {
private:
    string maCamBien;

    vector<NguoiQuanSat*> danhSachNguoiQuanSat;

public:
    CamBienIoT(const string& ma) {
        maCamBien = ma;
    }

    string getMaCamBien() const {
        return maCamBien;
    }

    void dangKy(NguoiQuanSat* nguoiQuanSat) {
        if (nguoiQuanSat != NULL) {
            danhSachNguoiQuanSat.push_back(
                nguoiQuanSat
            );
        }
    }

    // Huy dang ky mot Observer - hoan thien cap doi voi dangKy()
    // de dung dung tinh than Subject/Observer cua GoF.
    void huyDangKy(NguoiQuanSat* nguoiQuanSat) {
        danhSachNguoiQuanSat.erase(
            remove(
                danhSachNguoiQuanSat.begin(),
                danhSachNguoiQuanSat.end(),
                nguoiQuanSat
            ),
            danhSachNguoiQuanSat.end()
        );
    }

    void guiDuLieu(
        const DuLieuNhietDo& duLieu,
        const string& maThungLanh,
        const KhoangNhietDo& khoang
    ) {
        cout << "\n----------------------------------------\n";
        cout << "[CAM BIEN IOT] " << maCamBien << "\n";
        cout << "Thung lanh: " << maThungLanh << "\n";

        cout << fixed << setprecision(1);

        cout << "Nhiet do: "
             << duLieu.getNhietDo()
             << " C\n";

        cout << "Do am: "
             << duLieu.getDoAm()
             << " %\n";

        if (khoang.hopLe(duLieu.getNhietDo())) {

            cout << "Trang thai: BINH THUONG\n";

        } else {

            cout << "Trang thai: BAT THUONG\n";
            cout << "=> Cam bien phat tin hieu cho Observer...\n";

            size_t i;

            for (
                i = 0;
                i < danhSachNguoiQuanSat.size();
                i++
            ) {

                danhSachNguoiQuanSat[i]->xuLyCanhBao(
                    maCamBien,
                    maThungLanh,
                    duLieu,
                    khoang
                );
            }
        }
    }
};


// ======================================================
// 6. DIEM LO TRINH
// ======================================================

class DiemLoTrinh {
private:
    string tenDiaDiem;

public:
    DiemLoTrinh(const string& ten) {
        tenDiaDiem = ten;
    }

    string getTenDiaDiem() const {
        return tenDiaDiem;
    }
};


// ======================================================
// 7. LO TRINH
// ======================================================

class LoTrinh {
private:
    vector<DiemLoTrinh> danhSachDiem;

public:
    void themDiem(
        const DiemLoTrinh& diem
    ) {
        danhSachDiem.push_back(diem);
    }

    bool rong() const {
        return danhSachDiem.empty();
    }

    void hienThi() const {

        cout
            << "\n========== LO TRINH =========="
            << endl;

        if (danhSachDiem.empty()) {

            cout
                << "Chua co diem lo trinh."
                << endl;

            return;
        }

        size_t i;

        for (
            i = 0;
            i < danhSachDiem.size();
            i++
        ) {

            cout
                << i + 1
                << ". "
                << danhSachDiem[i]
                       .getTenDiaDiem()
                << endl;
        }
    }
};


// ======================================================
// 8. THUNG LANH
// ======================================================

class ThungLanh {
private:
    string maThungLanh;
    string loaiHang;
    double sucChuaKg;

    KhoangNhietDo khoangNhietDo;

    CamBienIoT* camBien;

public:

    ThungLanh(
        const string& ma,
        const string& loaiHang,
        double sucChua,
        const KhoangNhietDo& khoang,
        CamBienIoT* camBien
    )
        : khoangNhietDo(khoang)
    {
        maThungLanh = ma;
        this->loaiHang = loaiHang;
        sucChuaKg = sucChua;
        this->camBien = camBien;
    }

    // Ngan sao chep de tranh double-free (Rule of Three):
    // ThungLanh dang so huu con tro CamBienIoT* nen khong
    // duoc phep sao chep mac dinh (bitwise copy).
    ThungLanh(const ThungLanh&) = delete;
    ThungLanh& operator=(const ThungLanh&) = delete;

    ~ThungLanh() {
        delete camBien;
    }

    string getMaThungLanh() const {
        return maThungLanh;
    }

    void dangKyNguoiQuanSat(
        NguoiQuanSat* nguoiQuanSat
    ) {

        if (camBien != NULL) {

            camBien->dangKy(
                nguoiQuanSat
            );
        }
    }

    void huyDangKyNguoiQuanSat(
        NguoiQuanSat* nguoiQuanSat
    ) {

        if (camBien != NULL) {

            camBien->huyDangKy(
                nguoiQuanSat
            );
        }
    }

    void capNhatNhietDo(
        double nhietDo,
        double doAm
    ) {

        if (camBien == NULL) {
            return;
        }

        DuLieuNhietDo duLieu(
            nhietDo,
            doAm
        );

        camBien->guiDuLieu(
            duLieu,
            maThungLanh,
            khoangNhietDo
        );
    }

    void hienThi() const {

        cout
            << "  Ma thung lanh: "
            << maThungLanh
            << endl;

        cout
            << "  Loai hang: "
            << loaiHang
            << endl;

        cout
            << "  Suc chua: "
            << sucChuaKg
            << " kg"
            << endl;

        cout
            << "  Nhiet do cho phep: ["
            << khoangNhietDo.getToiThieu()
            << ", "
            << khoangNhietDo.getToiDa()
            << "] C"
            << endl;

        cout
            << "  Cam bien: "
            << (
                camBien != NULL
                ? camBien->getMaCamBien()
                : "Khong co"
            )
            << endl;
    }
};


// ======================================================
// 9. STATE PATTERN: TRANG THAI LO HANG
// ======================================================
// Thay vi dung enum + if/else de kiem tra trang thai (kieu
// cu), moi trang thai (MoiTao, TrongKho, DangVanChuyen,
// DaGiao) la 1 class rieng, cung implement 1 interface
// ITrangThaiLoHang. LoHang uy quyen hanh vi cho object
// trang thai hien tai, khong tu kiem tra trang thai bang tay.
//
// forward declare LoHang vi cac state can goi method tren no.
// ======================================================

class LoHang;

class ITrangThaiLoHang {
public:
    virtual ~ITrangThaiLoHang() {}

    virtual void duaVaoKho(LoHang& lo) = 0;
    virtual void batDauVanChuyen(LoHang& lo) = 0;
    virtual void giaoHang(LoHang& lo) = 0;

    virtual string tenTrangThai() const = 0;
};


class TrangThaiMoiTao : public ITrangThaiLoHang {
public:
    void duaVaoKho(LoHang& lo) override;
    void batDauVanChuyen(LoHang& lo) override;
    void giaoHang(LoHang& lo) override;

    string tenTrangThai() const override {
        return "MOI TAO";
    }

    static TrangThaiMoiTao& layInstance() {
        static TrangThaiMoiTao instance;
        return instance;
    }
};


class TrangThaiTrongKho : public ITrangThaiLoHang {
public:
    void duaVaoKho(LoHang& lo) override;
    void batDauVanChuyen(LoHang& lo) override;
    void giaoHang(LoHang& lo) override;

    string tenTrangThai() const override {
        return "TRONG KHO";
    }

    static TrangThaiTrongKho& layInstance() {
        static TrangThaiTrongKho instance;
        return instance;
    }
};


class TrangThaiDangVanChuyen : public ITrangThaiLoHang {
public:
    void duaVaoKho(LoHang& lo) override;
    void batDauVanChuyen(LoHang& lo) override;
    void giaoHang(LoHang& lo) override;

    string tenTrangThai() const override {
        return "DANG VAN CHUYEN";
    }

    static TrangThaiDangVanChuyen& layInstance() {
        static TrangThaiDangVanChuyen instance;
        return instance;
    }
};


class TrangThaiDaGiao : public ITrangThaiLoHang {
public:
    void duaVaoKho(LoHang& lo) override;
    void batDauVanChuyen(LoHang& lo) override;
    void giaoHang(LoHang& lo) override;

    string tenTrangThai() const override {
        return "DA GIAO";
    }

    static TrangThaiDaGiao& layInstance() {
        static TrangThaiDaGiao instance;
        return instance;
    }
};


// ======================================================
// 10. LO HANG - AGGREGATE ROOT
// ======================================================

class LoHang {
private:

    string maLoHang;
    string tenHangHoa;
    string khoXuat;
    string noiNhan;

    ITrangThaiLoHang* trangThaiHienTai;

    IChienLuocNguongNhietDo* chienLuocNhietDo;

    LoTrinh loTrinh;

    vector<ThungLanh*> danhSachThungLanh;

public:

    LoHang(
        const string& ma,
        const string& hangHoa,
        const string& kho,
        const string& noiNhan,
        IChienLuocNguongNhietDo& chienLuoc
    ) {
        maLoHang = ma;
        tenHangHoa = hangHoa;
        khoXuat = kho;
        this->noiNhan = noiNhan;

        chienLuocNhietDo = &chienLuoc;

        // Trang thai khoi tao mac dinh: MOI TAO
        trangThaiHienTai = &TrangThaiMoiTao::layInstance();
    }

    // Ngan sao chep de tranh double-free (LoHang dang so huu
    // cac con tro ThungLanh*).
    LoHang(const LoHang&) = delete;
    LoHang& operator=(const LoHang&) = delete;


    ~LoHang() {

        size_t i;

        for (
            i = 0;
            i < danhSachThungLanh.size();
            i++
        ) {

            delete danhSachThungLanh[i];
        }

        // Luu y: trangThaiHienTai va chienLuocNhietDo la
        // static instance (Singleton-like) dung chung giua
        // cac LoHang, nen KHONG duoc delete o day.
    }


    string getMaLoHang() const {
        return maLoHang;
    }


    // ---- Ho tro cho State Pattern ----

    string layTenTrangThai() const {
        return trangThaiHienTai->tenTrangThai();
    }

    bool laTrangThai(const ITrangThaiLoHang& trangThai) const {
        return trangThaiHienTai == &trangThai;
    }

    void chuyenTrangThai(ITrangThaiLoHang& trangThaiMoi) {
        trangThaiHienTai = &trangThaiMoi;
    }

    bool coThungLanh() const {
        return !danhSachThungLanh.empty();
    }

    bool coLoTrinh() const {
        return !loTrinh.rong();
    }


    // ---- Ho tro cho Strategy Pattern ----

    KhoangNhietDo getKhoangNhietDo() const {
        return chienLuocNhietDo->layKhoangNhietDo();
    }

    string layTenChienLuocNhietDo() const {
        return chienLuocNhietDo->tenChienLuoc();
    }


    void themThungLanh(
        ThungLanh* thungLanh
    ) {

        if (thungLanh != NULL) {

            danhSachThungLanh.push_back(
                thungLanh
            );
        }
    }


    void themDiemLoTrinh(
        const DiemLoTrinh& diem
    ) {

        loTrinh.themDiem(
            diem
        );
    }


    // SUA LOI VI PHAM AGGREGATE: ban goc tra ve tham chieu
    // KHONG const, cho phep code ben ngoai chinh sua truc tiep
    // vector noi bo (them/xoa ThungLanh ma LoHang khong kiem
    // soat duoc). Ham nay chi cho phep DOC, moi thay doi phai
    // di qua themThungLanh().
    const vector<ThungLanh*>&
    layDanhSachThungLanhChiDoc() const {

        return danhSachThungLanh;
    }


    // ---- Cac hanh dong nghiep vu: uy quyen cho State hien tai ----

    void duaVaoKho() {
        trangThaiHienTai->duaVaoKho(*this);
    }


    void batDauVanChuyen() {
        trangThaiHienTai->batDauVanChuyen(*this);
    }


    void giaoHang() {
        trangThaiHienTai->giaoHang(*this);
    }


    void hienThi() const {

        cout
            << "\n==================================================\n";

        cout
            << "                 THONG TIN LO HANG\n";

        cout
            << "==================================================\n";


        cout
            << "Ma lo hang       : "
            << maLoHang
            << endl;


        cout
            << "Ten hang hoa     : "
            << tenHangHoa
            << endl;


        cout
            << "Kho xuat phat    : "
            << khoXuat
            << endl;


        cout
            << "Noi nhan         : "
            << noiNhan
            << endl;


        cout
            << "Trang thai       : "
            << layTenTrangThai()
            << endl;


        cout
            << "Chien luoc nguong: "
            << layTenChienLuocNhietDo()
            << endl;


        KhoangNhietDo khoang = getKhoangNhietDo();

        cout
            << "Nhiet do cho phep: ["
            << khoang.getToiThieu()
            << ", "
            << khoang.getToiDa()
            << "] C"
            << endl;


        cout
            << "So thung lanh    : "
            << danhSachThungLanh.size()
            << endl;


        if (
            !danhSachThungLanh.empty()
        ) {

            cout
                << "\n--- DANH SACH THUNG LANH ---\n";

            size_t i;

            for (
                i = 0;
                i < danhSachThungLanh.size();
                i++
            ) {

                cout
                    << "\nThung lanh "
                    << i + 1
                    << ":\n";

                danhSachThungLanh[i]
                    ->hienThi();
            }
        }


        loTrinh.hienThi();


        cout
            << "==================================================\n";
    }
};


// ------------------------------------------------------
// Dinh nghia ngoai lop cua cac trang thai (State Pattern)
// Phai dat SAU khi LoHang da duoc dinh nghia day du, vi cac
// ham nay goi method tren LoHang.
// ------------------------------------------------------

void TrangThaiMoiTao::duaVaoKho(LoHang& lo) {

    lo.chuyenTrangThai(
        TrangThaiTrongKho::layInstance()
    );

    cout
        << "\n[LO HANG] Da dua lo hang vao kho."
        << endl;
}

void TrangThaiMoiTao::batDauVanChuyen(LoHang& lo) {
    (void) lo;

    cout
        << "\nLo hang con MOI TAO, "
        << "hay dua vao kho truoc."
        << endl;
}

void TrangThaiMoiTao::giaoHang(LoHang& lo) {
    (void) lo;

    cout
        << "\nKhong the giao hang khi "
        << "lo hang con MOI TAO."
        << endl;
}


void TrangThaiTrongKho::duaVaoKho(LoHang& lo) {
    (void) lo;

    cout
        << "\nLo hang da o trong kho roi."
        << endl;
}

void TrangThaiTrongKho::batDauVanChuyen(LoHang& lo) {

    if (!lo.coThungLanh()) {

        cout
            << "\nKhong the van chuyen khi "
            << "chua co thung lanh."
            << endl;

        return;
    }

    if (!lo.coLoTrinh()) {

        cout
            << "\nKhong the van chuyen khi "
            << "chua co lo trinh."
            << endl;

        return;
    }

    lo.chuyenTrangThai(
        TrangThaiDangVanChuyen::layInstance()
    );

    cout
        << "\n[LO HANG] Bat dau van chuyen."
        << endl;
}

void TrangThaiTrongKho::giaoHang(LoHang& lo) {
    (void) lo;

    cout
        << "\nChua the giao hang, "
        << "lo hang dang o trong kho."
        << endl;
}


void TrangThaiDangVanChuyen::duaVaoKho(LoHang& lo) {
    (void) lo;

    cout
        << "\nLo hang dang van chuyen, "
        << "khong the dua vao kho."
        << endl;
}

void TrangThaiDangVanChuyen::batDauVanChuyen(LoHang& lo) {
    (void) lo;

    cout
        << "\nLo hang da dang van chuyen roi."
        << endl;
}

void TrangThaiDangVanChuyen::giaoHang(LoHang& lo) {

    lo.chuyenTrangThai(
        TrangThaiDaGiao::layInstance()
    );

    cout
        << "\n[LO HANG] Da giao hang thanh cong."
        << endl;
}


void TrangThaiDaGiao::duaVaoKho(LoHang& lo) {
    (void) lo;

    cout
        << "\nLo hang da giao, "
        << "khong the thay doi trang thai."
        << endl;
}

void TrangThaiDaGiao::batDauVanChuyen(LoHang& lo) {
    (void) lo;

    cout
        << "\nLo hang da giao, "
        << "khong the van chuyen lai."
        << endl;
}

void TrangThaiDaGiao::giaoHang(LoHang& lo) {
    (void) lo;

    cout
        << "\nLo hang da giao roi."
        << endl;
}


// ======================================================
// 11. DICH VU CANH BAO - OBSERVER
// ======================================================

class DichVuCanhBao
    : public NguoiQuanSat
{
private:

    int soCanhBao;

public:

    DichVuCanhBao() {
        soCanhBao = 0;
    }


    virtual void xuLyCanhBao(

        const string& maCamBien,

        const string& maThungLanh,

        const DuLieuNhietDo& duLieu,

        const KhoangNhietDo& khoang

    ) {

        soCanhBao++;


        cout
            << "\n**************************************************\n";

        cout
            << "                !!! CANH BAO !!!\n";

        cout
            << "**************************************************\n";


        cout
            << "Lan canh bao      : "
            << soCanhBao
            << endl;


        cout
            << "Ma cam bien       : "
            << maCamBien
            << endl;


        cout
            << "Ma thung lanh     : "
            << maThungLanh
            << endl;


        cout << fixed << setprecision(1);


        cout
            << "Nhiet do hien tai : "
            << duLieu.getNhietDo()
            << " C"
            << endl;


        cout
            << "Nhiet do cho phep : ["
            << khoang.getToiThieu()
            << ", "
            << khoang.getToiDa()
            << "] C"
            << endl;


        cout
            << "Thoi gian         : "
            << duLieu.getThoiGian()
            << endl;


        cout
            << "Xu ly             : "
            << "KIEM TRA THUNG LANH NGAY!"
            << endl;


        cout
            << "**************************************************\n";
    }


    void hienThiTongSoCanhBao() const {

        cout
            << "\nTong so canh bao: "
            << soCanhBao
            << endl;
    }
};


// ======================================================
// 12. MO PHONG IOT
// ======================================================

class MoPhongIoT {
private:

    LoHang& loHang;

public:

    MoPhongIoT(
        LoHang& loHang
    )
        : loHang(loHang) {}


    void chay(
        int soLanDo
    ) {

        // Chi doc danh sach thung lanh (khong sua doi vector
        // noi bo cua Aggregate) - dung ham chi-doc moi da sua.
        const vector<ThungLanh*>& danhSach =
            loHang.layDanhSachThungLanhChiDoc();


        if (
            danhSach.empty()
        ) {

            cout
                << "Chua co thung lanh "
                << "de mo phong IoT."
                << endl;

            return;
        }


        int lan;


        for (
            lan = 1;
            lan <= soLanDo;
            lan++
        ) {

            cout
                << "\n\n========== LAN DO "
                << lan
                << " =========="
                << endl;


            size_t i;


            for (
                i = 0;
                i < danhSach.size();
                i++
            ) {

                // Nhiet do tu -5 den 12 C
                double nhietDo =
                    -5.0 +
                    (rand() % 171) / 10.0;


                // Do am tu 50 den 90 %
                double doAm =
                    50.0 +
                    (rand() % 401) / 10.0;


                danhSach[i]
                    ->capNhatNhietDo(
                        nhietDo,
                        doAm
                    );
            }
        }
    }
};


// ======================================================
// 13. MENU
// ======================================================

void hienThiMenu() {

    cout
        << "\n\n===============================================\n";

    cout
        << "     HE THONG QUAN LY CHUOI CUNG UNG LANH\n";

    cout
        << "===============================================\n";

    cout
        << "1. Tao lo hang\n";

    cout
        << "2. Them thung lanh + cam bien IoT\n";

    cout
        << "3. Them diem vao lo trinh\n";

    cout
        << "4. Xem thong tin lo hang\n";

    cout
        << "5. Dua lo hang vao kho\n";

    cout
        << "6. Bat dau van chuyen\n";

    cout
        << "7. Mo phong du lieu IoT\n";

    cout
        << "8. Xem tong so canh bao\n";

    cout
        << "9. Giao hang\n";

    cout
        << "10. Xoa lo hang hien tai\n";

    cout
        << "0. Thoat\n";

    cout
        << "===============================================\n";
}


void hienThiMenuChonChienLuoc() {

    cout
        << "\nChon chien luoc nguong nhiet do cho lo hang:\n";

    cout
        << "1. "
        << ChienLuocVacXin::layInstance().tenChienLuoc()
        << "\n";

    cout
        << "2. "
        << ChienLuocThucPhamTuoiSong::layInstance().tenChienLuoc()
        << "\n";

    cout
        << "3. "
        << ChienLuocThucPhamDongLanh::layInstance().tenChienLuoc()
        << "\n";
}


IChienLuocNguongNhietDo& chonChienLuocNhietDo() {

    hienThiMenuChonChienLuoc();

    int luaChon =
        nhapSoNguyen(
            "Nhap lua chon (1-3): ",
            1,
            3
        );

    switch (luaChon) {

        case 1:
            return ChienLuocVacXin::layInstance();

        case 2:
            return ChienLuocThucPhamTuoiSong::layInstance();

        default:
            return ChienLuocThucPhamDongLanh::layInstance();
    }
}


// ======================================================
// 14. MAIN
// ======================================================

int main() {

    srand(
        (unsigned int)time(NULL)
    );


    LoHang* loHang = NULL;


    DichVuCanhBao dichVuCanhBao;


    int luaChon;


    do {

        hienThiMenu();


        luaChon =
            nhapSoNguyen(
                "Nhap lua chon: ",
                0,
                10
            );


        switch (luaChon) {

            // ==========================================
            // 1. TAO LO HANG
            // ==========================================

            case 1:
            {
                if (loHang != NULL) {

                    cout
                        << "Da ton tai lo hang. "
                        << "Hay xoa lo hang hien tai truoc."
                        << endl;

                    break;
                }


                cout
                    << "\n========== TAO LO HANG ==========\n";


                string maLoHang =
                    nhapChuoi(
                        "Nhap ma lo hang: "
                    );


                string tenHangHoa =
                    nhapChuoi(
                        "Nhap ten hang hoa: "
                    );


                string khoXuat =
                    nhapChuoi(
                        "Nhap kho xuat phat: "
                    );


                string noiNhan =
                    nhapChuoi(
                        "Nhap noi nhan: "
                    );


                // STRATEGY PATTERN: chon chien luoc nguong
                // nhiet do theo loai hang, thay vi nguoi dung
                // tu nhap tay min/max nhu ban cu.
                IChienLuocNguongNhietDo& chienLuoc =
                    chonChienLuocNhietDo();


                loHang =
                    new LoHang(
                        maLoHang,
                        tenHangHoa,
                        khoXuat,
                        noiNhan,
                        chienLuoc
                    );


                cout
                    << "\n>>> Tao lo hang thanh cong!"
                    << endl;


                break;
            }


            // ==========================================
            // 2. THEM THUNG LANH
            // ==========================================

            case 2:
            {
                if (loHang == NULL) {

                    cout
                        << "Chua co lo hang. "
                        << "Hay tao lo hang truoc."
                        << endl;

                    break;
                }


                cout
                    << "\n===== THEM THUNG LANH =====\n";


                string maThungLanh =
                    nhapChuoi(
                        "Nhap ma thung lanh: "
                    );


                double sucChua =
                    nhapSoThuc(
                        "Nhap suc chua (kg): ",
                        0.1,
                        1000000
                    );


                string maCamBien =
                    nhapChuoi(
                        "Nhap ma cam bien IoT: "
                    );


                CamBienIoT* camBien =
                    new CamBienIoT(
                        maCamBien
                    );


                ThungLanh* thungLanh =
                    new ThungLanh(

                        maThungLanh,

                        "Theo lo hang",

                        sucChua,

                        loHang->getKhoangNhietDo(),

                        camBien
                    );


                // Dang ky Observer
                thungLanh
                    ->dangKyNguoiQuanSat(
                        &dichVuCanhBao
                    );


                loHang
                    ->themThungLanh(
                        thungLanh
                    );


                cout
                    << ">>> Them thung lanh thanh cong!"
                    << endl;


                break;
            }


            // ==========================================
            // 3. THEM DIEM LO TRINH
            // ==========================================

            case 3:
            {
                if (loHang == NULL) {

                    cout
                        << "Chua co lo hang."
                        << endl;

                    break;
                }


                cout
                    << "\n===== THEM DIEM LO TRINH =====\n";


                string diaDiem =
                    nhapChuoi(
                        "Nhap ten dia diem: "
                    );


                DiemLoTrinh diem(
                    diaDiem
                );


                loHang
                    ->themDiemLoTrinh(
                        diem
                    );


                cout
                    << ">>> Them diem lo trinh thanh cong!"
                    << endl;


                break;
            }


            // ==========================================
            // 4. XEM THONG TIN
            // ==========================================

            case 4:
            {
                if (loHang == NULL) {

                    cout
                        << "Chua co lo hang."
                        << endl;

                    break;
                }


                loHang
                    ->hienThi();


                break;
            }


            // ==========================================
            // 5. DUA VAO KHO
            // ==========================================

            case 5:
            {
                if (loHang == NULL) {

                    cout
                        << "Chua co lo hang."
                        << endl;

                    break;
                }


                loHang
                    ->duaVaoKho();


                break;
            }


            // ==========================================
            // 6. BAT DAU VAN CHUYEN
            // ==========================================

            case 6:
            {
                if (loHang == NULL) {

                    cout
                        << "Chua co lo hang."
                        << endl;

                    break;
                }


                loHang
                    ->batDauVanChuyen();


                break;
            }


            // ==========================================
            // 7. MO PHONG IOT
            // ==========================================

            case 7:
            {
                if (loHang == NULL) {

                    cout
                        << "Chua co lo hang."
                        << endl;

                    break;
                }


                if (
                    !loHang->laTrangThai(
                        TrangThaiDangVanChuyen::layInstance()
                    )
                ) {

                    cout
                        << "Hay dua lo hang vao kho "
                        << "va bat dau van chuyen truoc."
                        << endl;

                    break;
                }


                int soLan =
                    nhapSoNguyen(
                        "Nhap so lan cam bien gui du lieu (1-20): ",
                        1,
                        20
                    );


                cout
                    << "\n===== BAT DAU MO PHONG IOT =====\n";


                MoPhongIoT moPhong(
                    *loHang
                );


                moPhong.chay(
                    soLan
                );


                cout
                    << "\n===== KET THUC MO PHONG IOT =====\n";


                break;
            }


            // ==========================================
            // 8. XEM CANH BAO
            // ==========================================

            case 8:
            {
                dichVuCanhBao
                    .hienThiTongSoCanhBao();

                break;
            }


            // ==========================================
            // 9. GIAO HANG
            // ==========================================

            case 9:
            {
                if (loHang == NULL) {

                    cout
                        << "Chua co lo hang."
                        << endl;

                    break;
                }


                loHang
                    ->giaoHang();


                break;
            }


            // ==========================================
            // 10. XOA LO HANG
            // ==========================================

            case 10:
            {
                if (loHang == NULL) {

                    cout
                        << "Khong co lo hang de xoa."
                        << endl;

                    break;
                }


                delete loHang;

                loHang = NULL;


                cout
                    << ">>> Da xoa lo hang hien tai."
                    << endl;


                break;
            }


            // ==========================================
            // 0. THOAT
            // ==========================================

            case 0:

                cout
                    << "\nCam on ban da su dung he thong!"
                    << endl;

                break;
        }

    } while (
        luaChon != 0
    );


    if (loHang != NULL) {

        delete loHang;

        loHang = NULL;
    }


    return 0;
}