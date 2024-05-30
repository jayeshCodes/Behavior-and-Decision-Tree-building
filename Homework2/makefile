LIBS= -I./include -L./include -lsfml-graphics -lsfml-window -lsfml-system

all:
	g++ -c "VelocityMatching.cpp" -o part1.o $(LIBS)
	g++ -c "ArriveAndAlign.cpp" -o part2.o $(LIBS)
	g++ -c "Wander.cpp" -o part3.o $(LIBS)
	g++ -c "Flocking1.cpp" -o part4_1.o $(LIBS)
	g++ -c "Flocking2.cpp" -o part4_2.o $(LIBS)
	g++ -o part1 part1.o $(LIBS)
	g++ -o part2 part2.o $(LIBS)
	g++ -o part3 part3.o $(LIBS)
	g++ -o part4_1 part4_1.o $(LIBS)
	g++ -o part4_2 part4_2.o $(LIBS)

clean:
	rm -f part1.o part2.o part3.o part4_1.o part4_2.o part1 part2 part3 part4_1 part4_2