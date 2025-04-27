all:
	g++ -O3 -std=c++11 test.cpp -o test
	g++ -O3 -std=c++11 test1.cpp -o test1
	g++ -O3 -std=c++11 test2.cpp -o test2
paper:
	pdflatex paper.tex
