#   Automatic Outlet
### _Kelompok B2 Internet of Things-02_

Automatic Outlet adalah solusi inovatif dalam sistem otomatisasi rumah yang memanfaatkan platform Blynk untuk memberikan kontrol yang efisien dan pemantauan yang akurat terhadap perangkat elektronik. Proyek ini menggabungkan komponen termasuk sensor PZEM-004T, relay modul, dan mikrokontroler ESP32, untuk menciptakan sistem pintar yang dapat diakses dari jarak jauh.

    Group B2:
        - Ahmad Rifqi Fadhlurrahman        	- 2106731390
        - Zaki Ananda		          	- 2106636994
        - Alifya Zhafira Ananda		      	- 2106704111	
	- Cecilia Inez Reva Manurung          	- 2106636994

## Features

-   [Introduction to the Problem and the Solution](#i-introduction-to-the-problem-and-the-solution)
-   [Hardware Design and Implementation details](#ii-hardware-design-and-implementation-details)
-   [Network Infrastructure](#iii-network-infrastructure)
-   [Software Implementation Details](#iv-software-implementation-details)
-   [Test Results and Performance Evaluation](#v-test-results-and-performance-evaluation)
-   [Conclusion and Future Work](#vi-conclusion-and-future-work)

### Power Point
[![Link PPT Presentasi](https://img.shields.io/badge/Canva-%2300C4CC.svg?&style=for-the-badge&logo=Canva&logoColor=white)](https://www.canva.com/design/DAF2XW5AJp8/-UWWxGXYFD5tRD68L9yVOg/edit)

## i. Introduction to the problem and the solution

Automatic Outlet adalah perangkat otomatisasi rumah yang menggunakan platform Blynk untuk mengendalikan dan memonitor stop kontak adaptor. Proyek ini bertujuan memberikan pengguna kemampuan mengelola perangkat listrik secara efisien melalui aplikasi Blynk di smartphone. Menggunakan sensor PZEM-004T, relay modul, dan ESP32, proyek ini menjawab kebutuhan akan solusi ramah pengguna yang mengubah stop kontak tradisional menjadi perangkat pintar terkoneksi Internet of Things (IoT). Dengan kemampuan mengukur konsumsi daya dan kontrol jarak jauh, Automatic Outlet menjadi solusi terkini untuk memenuhi tuntutan rumah modern yang semakin otomatis dan terhubung.

## ii. Hardware design and implementation details

Peralatan yang dibutuhkan untuk merangkai Automatic Outlet :

### 1. ESP32
ESP32 adalah mikrokontroler WiFi dan Bluetooth untuk mengelola logika pengendalian, berkomunikasi dengan server Blynk, dan mengontrol relay untuk mengatur daya pada socket.
### 2. Relay Modul
Relay 2 channel dalam proyek ini berperan sebagai saklar elektronik yang dikendalikan oleh ESP32 untuk mengatur daya pada socket.
### 3. PZEM-004T
PZEM-004T adalah sensor untuk mengukur parameter daya, memantau konsumsi energi perangkat di socket pintar, memberikan informasi penting, dan memungkinkan pengguna melacak serta mengelola konsumsi daya perangkat dengan efisien.
### 4. Socket dan Plug
Socket adalah tempat perangkat listrik terhubung, sementara plug adalah konektor untuk menghubungkannya; keduanya berperan sebagai antarmuka fisik yang dikelola oleh relay dalam sistem otomatisasi.
### 5. Adaptor
Adaptor memberikan daya stabil kepada ESP32 dan komponen lainnya untuk menjaga operasional mikrokontroler.


## iii. Network Infrastructure

![network infrastructure](https://github.com/alifyaza/Automatic-Outlet/assets/87703952/f2ee4986-d67d-4063-b01a-cb4884f47557)

## iv. Software implementation details

Berikut flowchart secara keseluruhan mengenai program yang dibuat:
![flowIoT](https://github.com/alifyaza/Automatic-Outlet/assets/87703952/1d9f38ae-c089-4e05-ba42-5484c65e19c6)

SEQUENCE DIAGRAM
![sequence diagram](https://github.com/alifyaza/Automatic-Outlet/assets/88533766/a3b53de2-5774-4252-817c-cb2fc87975cc)

STATE DIAGRAM
![state diagram](https://github.com/alifyaza/Automatic-Outlet/assets/88533766/84e95b3e-78f6-4550-ac57-d99322f096b5)

## v. Test results and performance evaluation

Test Result :
![Hasil](https://github.com/alifyaza/Automatic-Outlet/assets/88533766/d32b0ee8-8b7c-4cf7-9375-d39fe71b0783)
![blynk](https://github.com/alifyaza/Automatic-Outlet/assets/88533766/fc6cfa3d-378a-4c27-9fbb-a9c7eee7e72b)

Performance Evaluation :
1. Input Responsif
   - Input '1' pada Blynk menghidupkan perangkat (isOn = true).
   - Input '0' pada Blynk mematikan perangkat (isOn = false).
   - Estimasi waktu respons perangkat dari saat penekanan tombol 'on' melalui interface Blynk hingga relay sepenuhnya teraktivasi adalah antara 1 hingga 2 detik.


2. Kontrol Relay
   - Relay berperan sebagai switch daya pada smart outlet.
   - Pengujian menunjukkan relay dapat menghidupkan (aktif) atau mematikan (non-aktif) smart outlet.

3. Sensor PZEM-004T
   - Sensor memberikan data yang akurat dan responsif.
   - Pemantauan sensor hanya aktif saat perangkat dalam keadaan hidup (isOn = true).

4. Pengiriman Data ke Blynk
   - Data konsumsi daya dan status perangkat terkirim dengan sukses ke server Blynk.
   - Informasi di aplikasi Blynk sesuai dengan kondisi aktual perangkat.

5. Fungsi Waktu dan NTP
   - Sinkronisasi waktu dan NTP berjalan dengan baik.
   - Perangkat dapat dimatikan otomatis sesuai waktu yang diatur pengguna di aplikasi Blynk.
     

## vi. Conclusion and Future Work

Proyek "Automatic Outlet" menggunakan mikrokontroler ESP32, relay 2 Channel, dan sensor PZEM-004T untuk memberikan solusi efisien dalam pemantauan dan pengendalian aliran daya listrik ke perangkat elektronik di dalam ruangan. Sistem ini terus-menerus memeriksa konsumsi daya perangkat dan status relay untuk mengatur aliran daya. Relay diaktifkan untuk menyalakan outlet dan dimatikan untuk mematikan perangkat. Sensor PZEM-004T memberikan pemantauan konsumsi daya yang akurat. Dengan antarmuka Blynk, proyek ini memberikan kontrol jarak jauh dan pemantauan waktu nyata, memberikan informasi real-time tentang penggunaan daya perangkat melalui aplikasi Blynk. Meskipun sederhana, integrasi yang baik antara perangkat keras dan perangkat lunak menciptakan solusi yang efisien dan mudah diakses untuk mengelola daya listrik di dalam ruangan.
