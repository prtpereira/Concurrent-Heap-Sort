N=1000000
MAX=32

for ths in $(seq 1 32)
do
  echo $ths
  ./heap "${ths}"
done

./heap 32
