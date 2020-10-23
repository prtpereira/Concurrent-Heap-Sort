N=1000000
MAX=32
#
# for ths in $(seq 1 1500 30000)
# do
#   echo "ths: $ths"
#   ./heap "$ths"
# done


for ths in $(seq 1 $MAX)
do
  FILE=javaperf_$ths.out
  sudo perf stat -o "$FILE" -B -e  cache-references,cache-misses,cycles,instructions,branches,faults,migrations java Sort $ths $N >> "$FILE".javaout
done
