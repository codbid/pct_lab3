set termoption enhanced
set terminal svg size 800,600 font "Arial, 16"
set encoding utf8
set output "1.svg"
set style line 1 lc rgb "0x6495ED" lt 1 lw 4 pt 7 ps 1
set border lw 2
set grid
set key top left
set xlabel "Число потоков"
set ylabel "Коэффициент ускорения" rotate by 90
set xtics 2
set format y "%1.4f"
plot "1.dat" using 1:2 title "Параллельное численное интегрирование методом средних прямоугольников" with linespoints ls 1, \