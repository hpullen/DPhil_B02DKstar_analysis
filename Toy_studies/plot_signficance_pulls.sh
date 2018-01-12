# Code to plot significance pulls and write results to a sorted file
NAME=$1
./PlotSignificancePulls ${NAME} | tee logs/${NAME}.log
cat logs/${NAME}.log | tail -28 >> pull_lists/${NAME}_unsorted.pulls
sort -k2nr pull_lists/${NAME}.pulls >> pull_lists/${NAME}_sorted.pulls
column -t pull_lists/${NAME}_sorted.pulls >> pull_lists/${NAME}.pulls
rm pull_lists/${NAME}_unsorted.pulls
rm pull_lists/${NAME}_sorted.pulls
