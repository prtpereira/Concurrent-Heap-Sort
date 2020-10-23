N=1000000
MAX=32

for ths in $(seq 1 $MAX)
do
  FILE=cpp_perf_$ths.out
  sudo perf stat -o "$FILE" -B -e  cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./heap "$ths"
  sleep 1
done


echo "hello"
