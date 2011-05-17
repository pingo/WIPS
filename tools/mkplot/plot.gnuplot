set encoding utf8
set term postscript eps color blacktext "Helvetica" 12

unset multiplot

set autoscale
set xtic auto
set ytic auto
set xlabel 'Time'
set timefmt "%s"
set xdata time
set format x "%H:%M"
set ylabel 'Time (s)'

set yrange [-5:60]

set output 'eps/rtt_retry.eps'
set title "Round Trip Time"

plot "< egrep '^72.0 ' rtt_retry.txt" using 2:3 title '72.0' with lines, \
     "< egrep '^74.0 ' rtt_retry.txt" using 2:3 title '74.0' with lines

# set xrange [0:*]
# set yrange [350:600]
set yrange [-5:60]

set output 'eps/rtt_perfect.eps'
set title "Round Trip Time (drops filtered out)"

plot "< egrep '^72.0 ' rtt_perfect.txt" using 2:3 title '72.0' with lines, \
     "< egrep '^74.0 ' rtt_perfect.txt" using 2:3 title '74.0' with lines
