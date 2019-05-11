# FP_SISOP19_E14

# format config
`menit(0-59) jam(0-23) tanggal(1-31) bulan(1-12) hari(0-6; 0=minggu,1=senin,dst.) path`

contoh: `* * * * * /home/somi/test.sh`

(test.sh akan dijalankan tiap menit)

# penjelasan
file `crontab.data` disimpan di folder `/home/user/`. buat program daemon yang berjalan tiap menit (sleep 60 detik), kemudian bandingkan waktu sekarang dengan kriteria waktu config jika sesuai maka buat thread yang berfungsi untuk menjalankan bash script. thread tersebut akan membuat proses child independen baru agar bisa memanggil system call exec() yang berfungsi untuk memanggil command bash. sebelum membuat proses child baru, set signal SIGCHLD menjadi SIG_IGN agar proses child yang akan dibuat bisa di terminasi sesudah menjalankan bash script tanpa harus menunggu signal wait dari proses parent nya (menghindari lahirnya proses zombie).
