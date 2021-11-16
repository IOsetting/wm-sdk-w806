#!/bin/sh
ProjName="W806"
signature=0
prikey_sel=0
code_encrypt=0
sign_pubkey_src=0
img_type=1
zip_type=1

sec_img_header=8002000
sec_img_pos=8002400
run_img_header=8010000
run_img_pos=8010400
upd_img_pos=8010000


echo $ProjName
if [ $prikey_sel -gt 0 ]
 then
  let img_type=$img_type+32*$prikey_sel
fi

if [ $code_encrypt -eq 1 ]
 then
  let img_type=$img_type+16
fi

if [ $signature -eq 1 ]
 then
  let img_type=$img_type+256
fi

if [ $sign_pubkey_src -eq 1 ]
 then
  let img_type=$img_type+512
fi

echo $img_type

csky-elfabiv2-objcopy -O binary ./"$ProjName".elf ./"$ProjName".bin

if [ $code_encrypt -eq 1 ]
 then
  let prikey_sel=$prikey_sel+1
  openssl enc -aes-128-ecb -in ./"$ProjName".bin -out ./"$ProjName"_enc.bin -K 30313233343536373839616263646566 -iv 01010101010101010101010101010101
  openssl rsautl -encrypt -in ../../../../../tools/W806/ca/key.txt -inkey ../../../../../tools/W806/ca/capub_"$prikey_sel".pem -pubin -out key_en.dat
  cat "$ProjName"_enc.bin key_en.dat > "$ProjName"_enc_key.bin
  cat "$ProjName"_enc_key.bin ../../../../../tools/W806/ca/capub_"$prikey_sel"_N.dat > "$ProjName"_enc_key_N.bin  
  ../../../../../../../tools/W806/wm_tool.exe -b ./"$ProjName"_enc_key_N.bin -o ./"$ProjName" -it $img_type -fc 0 -ra $run_img_pos -ih $run_img_header -ua $upd_img_pos -nh 0  -un 0
 else
  ../../../../../../../tools/W806/wm_tool.exe -b ./"$ProjName".bin -o ./"$ProjName" -it $img_type -fc 0 -ra $run_img_pos -ih $run_img_header -ua $upd_img_pos -nh 0  -un 0
fi

mkdir -p ../../../../../../../bin/W806
mv ./"$ProjName".map ../../../../../../../bin/W806/"$ProjName".map

if [ $signature -eq 1 ]
 then
  openssl dgst -sign  ../../../../../../../tools/W806/ca/cakey.pem -sha1 -out "$ProjName"_sign.dat ./"$ProjName".img
  cat "$ProjName".img "$ProjName"_sign.dat > "$ProjName"_sign.img
  mv ./"$ProjName"_sign.img ../../../../../../../bin/W806/"$ProjName"_sign.img

  #when you change run-area image's ih, you must remake secboot img with secboot img's -nh address same as run-area image's ih
  ../../../../../../../tools/W806/wm_tool.exe -b ../../../../../../../tools/W806/W806_secboot.bin -o ../../../../../../../tools/W806/W806_secboot -it 0 -fc 0 -ra $sec_img_pos -ih $sec_img_header -ua $upd_img_pos -nh $run_img_header  -un 0    
  cat ../../../../../../../tools/W806/"$ProjName"_secboot.img ../../../../../../../bin/W806/"$ProjName"_sign.img > ../../../../../../../bin/W806/"$ProjName".fls  
 else
  mv ./"$ProjName".img ../../../../../../../bin/W806/"$ProjName".img
  
  #when you change run-area image's ih, you must remake secboot img with secboot img's -nh address same as run-area image's ih
  ../../../../../../../tools/W806/wm_tool.exe -b ../../../../../../../tools/W806/W806_secboot.bin -o ../../../../../../../tools/W806/W806_secboot -it 0 -fc 0 -ra $sec_img_pos -ih $sec_img_header -ua $upd_img_pos -nh $run_img_header  -un 0  
  cat ../../../../../../../tools/W806/"$ProjName"_secboot.img ../../../../../../../bin/W806/"$ProjName".img > ../../../../../../../bin/W806/"$ProjName".fls
fi

#produce compressed ota firmware*/
if [ $zip_type -eq 1 ]
 then
  if [ $signature -eq 1 ]
   then
    ../../../../../../../tools/W806/wm_tool.exe -b ./"$ProjName"_sign.img -o ../../../../../../../bin/W806/"$ProjName"_sign -it $img_type -fc 1 -ra $run_img_pos -ih $run_img_header -ua $upd_img_pos -nh 0  -un 0
    mv ../../../../../../../bin/W806/"$ProjName"_sign_gz.img ../../../../../../../bin/W806/"$ProjName"_sign_ota.img
  else
   ../../../../../../../tools/W806/wm_tool.exe -b ../../../../../../../bin/W806/"$ProjName".img -o ../../../../../../../bin/W806/"$ProjName" -it $img_type -fc 1 -ra $run_img_pos -ih $run_img_header -ua $upd_img_pos -nh 0  -un 0
   mv ../../../../../../../bin/W806/"$ProjName"_gz.img ../../../../../../../bin/W806/"$ProjName"_ota.img
  fi
fi
#openssl --help