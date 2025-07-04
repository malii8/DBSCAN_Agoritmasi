#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <fstream>

// Ortamlar sınıfı
class Ortamlar {
public:
    // CSV dosyasını oku ve veri matrisine dönüştür
    std::vector<std::vector<double>> CsvOku(const std::string& dosyaYolu, bool baslikVar = true) {
        std::vector<std::vector<double>> tumVeriler;
        std::ifstream dosya(dosyaYolu);

        if (!dosya.is_open()) {
            std::cerr << "Dosya açılamadı: " << dosyaYolu << std::endl;
            return tumVeriler;
        }

        std::string satir;
        bool ilkSatir = true;

        while (std::getline(dosya, satir)) {
            // Başlık satırını atla
            if (ilkSatir && baslikVar) {
                ilkSatir = false;
                continue;
            }

            std::vector<double> veriSatiri;
            size_t pozisyon = 0;
            size_t yeniPozisyon = 0;

            // Virgülle ayrılmış değerleri parse et
            while ((yeniPozisyon = satir.find(',', pozisyon)) != std::string::npos) {
                std::string deger = satir.substr(pozisyon, yeniPozisyon - pozisyon);
                // Nokta ve virgül formatı dönüşümü
                std::replace(deger.begin(), deger.end(), '.', ',');
                veriSatiri.push_back(fabs(std::stod(deger)));
                pozisyon = yeniPozisyon + 1;
            }

            // Son değeri ekle
            std::string deger = satir.substr(pozisyon);
            std::replace(deger.begin(), deger.end(), '.', ',');
            veriSatiri.push_back(fabs(std::stod(deger)));

            tumVeriler.push_back(veriSatiri);
        }

        dosya.close();
        return tumVeriler;
    }

    // Veriyi ekrana yazdır
    void VeriGoster(const std::string& baslik, const std::vector<std::vector<double>>& veri, int hucreGenisligi = 7) {
        int indeks = 1;

        for (const auto& satir : veri) {
            std::cout << baslik << " " << indeks << ":";

            if (indeks < 10) {
                std::cout << " ";
            }

            std::cout << " |  ";
            indeks++;

            for (const auto& hucre : satir) {
                std::cout << hucre;
                // Hücre genişliğine göre boşluk ekle
                int boslukSayisi = hucreGenisligi - std::to_string(hucre).length();
                for (int i = 0; i < boslukSayisi; i++) {
                    std::cout << " ";
                }
            }

            std::cout << std::endl;
        }
    }
};

// DbscanAraclari sınıfı
class DbscanAraclari {
public:
    // İki nokta arasındaki Öklid mesafesini hesaplar
    double OklidMesafe(const std::vector<double>& nokta1, const std::vector<double>& nokta2) {
        double mesafe = 0;
        for (size_t i = 0; i < nokta1.size(); i++) {
            mesafe += pow(nokta1[i] - nokta2[i], 2);
        }
        return round(sqrt(mesafe) * 100) / 100; // 2 basamak yuvarla
    }

    // Bir noktanın komşularını bulur
    std::vector<int> KomsulariBul(const std::vector<std::vector<double>>& veri, int noktaIndeks,
        double epsilon, const std::vector<int>* atlaListesi = nullptr) {
        std::vector<int> komsular;
        int boyut = veri.size();
        double mesafe = 0;

        for (int i = 0; i < boyut; i++) {
            // Kendisi veya atlanacak listede olan noktaları atlama
            if (i == noktaIndeks || (atlaListesi && std::find(atlaListesi->begin(), atlaListesi->end(), i) != atlaListesi->end()))
                continue;

            mesafe = OklidMesafe(veri[noktaIndeks], veri[i]);
            if (mesafe <= epsilon) {
                komsular.push_back(i);
            }
        }

        return komsular;
    }

    // DBSCAN kümeleme algoritması
    std::vector<int> DBSCAN(const std::vector<std::vector<double>>& veri, double epsilon, int minNokta) {
        int kume_id = 1;
        std::vector<int> etiketler(veri.size(), -1);

        for (size_t i = 0; i < veri.size(); i++) {
            // Eğer nokta zaten bir kümeye atanmışsa atla
            if (etiketler[i] != -1)
                continue;

            // Noktanın komşularını bul
            std::vector<int> komsular = KomsulariBul(veri, i, epsilon);
            komsular.insert(komsular.begin(), i); // Kendisini de ekle

            if (komsular.size() == 1) {
                std::cout << "Nokta " << i + 1 << " hiç komşuya sahip değil." << std::endl;
            }
            else {
                std::cout << "Nokta " << i + 1 << " komşuları: ";
                for (auto nokta : komsular) {
                    std::cout << nokta + 1 << " ";
                }
                std::cout << std::endl;
            }

            if (komsular.size() > 1) {
                // Tüm komşuların komşularını bul
                for (size_t j = 1; j < komsular.size(); j++) {
                    std::vector<int> noktaKomsulari = KomsulariBul(veri, komsular[j], epsilon, &komsular);

                    if (!noktaKomsulari.empty()) {
                        for (auto nokta : noktaKomsulari) {
                            if (std::find(komsular.begin(), komsular.end(), nokta) == komsular.end()) {
                                komsular.push_back(nokta);
                                std::cout << "Nokta " << komsular[j] + 1 << " komşusu: Nokta " << nokta + 1 << " eklendi." << std::endl;
                            }
                        }
                    }
                    else {
                        std::cout << "Nokta " << komsular[j] + 1 << " eklenecek komşu bulunamadı." << std::endl;
                    }
                }

                // Eğer küme yeterince büyükse, bir küme oluştur
                if (komsular.size() > minNokta) {
                    std::cout << "Küme " << kume_id << " toplam " << komsular.size() << " noktaya sahip." << std::endl;

                    for (auto& nokta : komsular) {
                        
                            etiketler[nokta] = kume_id;
                        
                    }

                    std::cout << "Küme " << kume_id << " tamamlandı.\n" << std::endl;
                    kume_id++;
                }
                else {
                    std::cout << "Toplam komşu sayısı " << komsular.size() << " nokta.\nYeni küme oluşturulmadı.\n" << std::endl;
                }
            }
            else {
                etiketler[i] = -2; // Gürültü olarak işaretle
                std::cout << i + 1 << " gürültü olarak işaretlendi.\n" << std::endl;
                continue;
            }
        }

        // Kümelenmeyen noktaları gürültü olarak işaretle
        for (size_t i = 0; i < etiketler.size(); i++) {
            if (etiketler[i] == -1) {
                etiketler[i] = -2;
                std::cout << i + 1 << " gürültü olarak işaretlendi." << std::endl;
            }
        }

        return etiketler;
    }

    // Kümelerin eleman sayılarını bulma
    void ElemanSayilariniBul(const std::vector<int>& kumeEtiketleri) {
        std::vector<int> sayilar;
        int maksKumeId = *std::max_element(kumeEtiketleri.begin(), kumeEtiketleri.end());

        for (int i = 1; i <= maksKumeId; i++) {
            int sayac = std::count(kumeEtiketleri.begin(), kumeEtiketleri.end(), i);
            sayilar.push_back(sayac);
        }

        int indeks = 1;
        for (auto sayi : sayilar) {
            std::cout << "Küme " << indeks << " toplam " << sayi << " elemana sahip." << std::endl;
            std::cout << "Eleman listesi:" << std::endl;

            for (size_t j = 0; j < kumeEtiketleri.size(); j++) {
                if (kumeEtiketleri[j] == indeks) {
                    std::cout << j + 1 << " ";
                }
            }

            std::cout << "\n" << std::endl;
            indeks++;
        }

        // Gürültü elemanları sayma
        int gurultuSayisi = std::count(kumeEtiketleri.begin(), kumeEtiketleri.end(), -2);

        std::cout << gurultuSayisi << " eleman gürültü olarak işaretlendi." << std::endl;
        std::cout << "Gürültü elemanları:" << std::endl;

        for (size_t i = 0; i < kumeEtiketleri.size(); i++) {
            if (kumeEtiketleri[i] == -2) {
                std::cout << i + 1 << " ";
            }
        }

        std::cout << "\n" << std::endl;
    }
};

// Ana program
int main() {
    // Türkçe karakter desteği için
    setlocale(LC_ALL, "Turkish");

    Ortamlar ortam;
    DbscanAraclari dbscan;

    // CSV dosyasından veriyi oku
    std::vector<std::vector<double>> veri = ortam.CsvOku("heart_failure_clinical_records_dataset.csv");

    // Okunan veriyi ekrana yazdır
    ortam.VeriGoster("Veri", veri, 11);

    std::cout << "\nLütfen epsilon değerini giriniz: ";
    double epsilon;
    std::cin >> epsilon;

    std::cout << "\nLütfen minimum nokta sayısını giriniz: ";
    int minNokta;
    std::cin >> minNokta;

    // DBSCAN algoritmasını çalıştır
    std::vector<int> kumeler = dbscan.DBSCAN(veri, epsilon, minNokta);

    // Sonuçları göster
    std::cout << "\nKümeler:" << std::endl;
    for (size_t i = 0; i < kumeler.size(); i++) {
        if (kumeler[i] == -2) {
            std::cout << "Veri " << i + 1 << " -> Gürültü" << std::endl;
        }
        else {
            std::cout << "Veri " << i + 1 << " -> Küme: " << kumeler[i] << std::endl;
        }
    }

    std::cout << "\n__Küme eleman sayıları__" << std::endl;
    dbscan.ElemanSayilariniBul(kumeler);

    return 0;
}