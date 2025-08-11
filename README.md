# STM32F4 ADC DMA Projesi

Bu proje, STM32F4 mikrodenetleyici üzerinde ADC verilerini DMA ile okuyup UART üzerinden gönderen bir uygulamadır.

## 📋 Proje Özellikleri

- **ADC Okuma**: 4 kanal (PA0, PA1, PA4, PA5) ADC verisi
- **DMA Transfer**: Yüksek performanslı veri transferi
- **UART İletişimi**: 115200 baud rate ile veri gönderimi
- **Timer Kontrolü**: TIM1, TIM2, TIM5 zamanlayıcıları
- **Mesaj Formatları**: MessagePack ve ULog desteği

## 🛠 Gereksinimler

### Donanım
- STM32F4 Development Board (örn: STM32F4 Discovery, Nucleo-F4xx)
- ST-Link programlayıcı (genelde boardlarda dahili)
- UART/USB adapter (debugging için)

### Yazılım 
- ARM GCC Toolchain ✅ (kurulu)
- STM32CubeIDE (opsiyonel, GUI için)
- OpenOCD ✅ (kurulu)
- ST-Link tools ✅ (kurulu)

## 🚀 Çalıştırma Yöntemleri

### Yöntem 1: STM32 Board ile (Önerilen)
```bash
# Derleme
make -f Makefile_simple

# STM32 board'a flash etme
st-flash write simple_test.bin 0x8000000

# Debug (opsiyonel)
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg &
arm-none-eabi-gdb simple_test.elf
```

### Yöntem 2: QEMU Simülasyon
```bash
# QEMU kurulumu
sudo apt install qemu-system-arm

# STM32F4 simülasyonu
qemu-system-arm -M stm32vldiscovery -kernel simple_test.elf -nographic
```

### Yöntem 3: STM32CubeIDE
1. STM32CubeIDE'yi indirin: https://www.st.com/en/development-tools/stm32cubeide.html
2. Yeni STM32 projesi oluşturun
3. Bu dosyaları projeye kopyalayın
4. Build ve Run yapın

## 📁 Dosya Yapısı

```
/workspace/
├── main.cpp                    # Ana uygulama kodu
├── main.h                      # Header dosyası
├── stm32f4xx_hal.h            # HAL kütüphane stub'ları
├── stm32f4xx_hal.c            # HAL fonksiyon implementasyonları
├── AdcDmaController.hpp        # ADC DMA kontrol sınıfı
├── msgPackSerializer.hpp       # MessagePack serializer
├── msgPackSerializer_ADC.h     # ADC için MessagePack
├── ULogSerializer_ADC.hpp      # ULog serializer
├── Makefile_simple            # Basit derleme dosyası
└── README.md                  # Bu dosya
```

## ⚙️ Kod Açıklaması

### Ana Fonksiyonlar
- `main()`: Sistem başlatma ve ana döngü
- `getTimestamp()`: 64-bit zaman damgası üretimi
- `HAL_TIM_PeriodElapsedCallback()`: Timer interrupt handler
- `HAL_ADC_ConvCpltCallback()`: ADC dönüşüm tamamlanma callback'i

### Konfigürasyon
- **REAL_TIME_COM**: MessagePack (1) veya ULog (0) formatı seçimi
- **Clock**: 84MHz sistem saati
- **ADC**: 4 kanal, DMA ile sürekli okuma
- **UART**: 115200 baud, 8N1

## 🔧 Derleme Detayları

Proje ARM Cortex-M4 için derlenmiştir:
- **CPU**: Cortex-M4
- **FPU**: Floating point unit enabled
- **Thumb instruction set**
- **C++20 standart**

## ⚡ Performans

- ADC örnekleme hızı: Timer1 ile kontrol edilir (yaklaşık 50Hz)
- DMA transfer: Otomatik, CPU müdahalesi minimum
- UART: 115200 bps veri hızı

## 🐛 Debugging

### Serial Monitor
```bash
# UART çıkışını izleme (Linux)
minicom -D /dev/ttyUSB0 -b 115200

# Windows
# PuTTY veya TeraTerm kullanın
```

### GDB Debugging
```bash
# Terminal 1: OpenOCD başlat
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# Terminal 2: GDB bağlan
arm-none-eabi-gdb simple_test.elf
(gdb) target extended-remote localhost:3333
(gdb) load
(gdb) monitor reset halt
(gdb) continue
```

## 📝 Notlar

- Bu implementasyon test ve geliştirme amaçlıdır
- Gerçek donanımda çalıştırmadan önce pin konfigürasyonlarını kontrol edin
- HAL fonksiyonları stub olarak implement edilmiştir
- Production kullanımı için tam HAL kütüphanesi gereklidir

## 🤝 Katkıda Bulunma

1. Bu projeyi fork edin
2. Feature branch oluşturun
3. Değişikliklerinizi commit edin
4. Pull request açın

## 📄 Lisans

Bu proje MIT lisansı altında dağıtılmaktadır.

---

**Not**: Bu kod embedded systems geliştirme ve eğitim amaçlı hazırlanmıştır.