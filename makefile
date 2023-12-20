default:
	g++ source/main.cpp -O3
	@clear && ./a.out data/Almanac.alm 2023-02-23-00-00-00 2023-02-23-00-30-00 60 52 21 100 10 -- GPS

test:
	./a.out data/Almanac.alm 2023-02-23-00-00-00 2023-02-23-00-15-00 60 52 21 100 10 -- 1

clean:
	rm -rf a.out *.png