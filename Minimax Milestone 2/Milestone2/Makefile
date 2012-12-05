# General test makefile for multi-target test
# Copyright Clinton Staley 12/1/01


# General definitions
CPP = g++
CPPFLAGS = -w -O3

MANCALAOBJS = MancalaBoard.o MancalaMove.o MancalaView.o MancalaDlg.o
OTHELLOOBJS = OthelloBoard.o OthelloMove.o OthelloView.o OthelloDlg.o
PYLOSOBJS = PylosBoard.o PylosMove.o PylosView.o PylosDlg.o
GAMEOBJS = Board.o Dialog.o Class.o $(MANCALAOBJS) $(OTHELLOOBJS) $(PYLOSOBJS)
BOARDTESTOBJS = BoardTest.o $(GAMEOBJS)
MYBOARDTESTOBJS = MyBoardTest.o $(GAMEOBJS)
MAKEBOOKOBJS = MakeBook.o Book.o SimpleAIPlayer.o BestMove.o $(GAMEOBJS)

MakeBook : $(MAKEBOOKOBJS)
	$(CPP) $(MAKEBOOKOBJS) -o MakeBook

MyBoardTest : $(MYBOARDTESTOBJS)
	$(CPP) $(MYBOARDTESTOBJS) -o MyBoardTest

BoardTest : $(BOARDTESTOBJS)
	$(CPP) $(BOARDTESTOBJS) -o BoardTest

BTRelease: BoardTest BoardTest.o
	chmod 755 BoardTest
	chmod 644 BoardTest.o
	mv BoardTest BoardTest.o ../../prj1

MBRelease: MakeBook
	chmod 755 MakeBook
	mv MakeBook ../../prj2

clean:
	rm BoardTest MyBoardTest *.o

# Buggy version dependencies and definitions
GAMEOBJSB0 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(PYLOSOBJS) MancalaBoard.o MancalaMoveB0.o MancalaView.o MancalaDlg.o

GAMEOBJSB1 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(PYLOSOBJS) MancalaBoardB1.o MancalaMove.o MancalaView.o MancalaDlg.o 

GAMEOBJSB2 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(PYLOSOBJS) MancalaBoardB2.o MancalaMove.o MancalaView.o MancalaDlg.o 

GAMEOBJSB3 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(PYLOSOBJS) MancalaBoardB3.o MancalaMove.o MancalaView.o MancalaDlg.o 

GAMEOBJSB4 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(PYLOSOBJS) MancalaBoardB4.o MancalaMove.o MancalaView.o MancalaDlg.o 

GAMEOBJSB5 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoardB5.o PylosMove.o PylosView.o PylosDlg.o

GAMEOBJSB6 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoardB6.o PylosMove.o PylosView.o PylosDlg.o

GAMEOBJSB7 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoardB7.o PylosMove.o PylosView.o PylosDlg.o

GAMEOBJSB8 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoardB8.o PylosMove.o PylosView.o PylosDlg.o

GAMEOBJSB9 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoardB9.o PylosMove.o PylosView.o PylosDlg.o

GAMEOBJSB10 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoardB10.o PylosMove.o PylosView.o PylosDlg.o

GAMEOBJSB11 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoard.o PylosMoveB11.o PylosView.o PylosDlg.o

GAMEOBJSB12 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoard.o PylosMoveB12.o PylosView.o PylosDlg.o

GAMEOBJSB13 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoard.o PylosMoveB13.o PylosView.o PylosDlg.o

GAMEOBJSB14 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoard.o PylosMoveB14.o PylosView.o PylosDlg.o

GAMEOBJSB15 = BoardTest.o Board.o Dialog.o Class.o $(OTHELLOOBJS) $(MANCALAOBJS) PylosBoard.o PylosMove.o PylosView.o PylosDlgB15.o

MancalaMoveB0.o : MancalaMove.cpp
	$(CPP) -c -DBUG0 MancalaMove.cpp -o MancalaMoveB0.o

MancalaBoardB1.o : MancalaBoard.cpp
	$(CPP) -c -DBUG1 MancalaBoard.cpp -o MancalaBoardB1.o

MancalaBoardB2.o : MancalaBoard.cpp
	$(CPP) -c -DBUG2 MancalaBoard.cpp -o MancalaBoardB2.o

MancalaBoardB3.o : MancalaBoard.cpp
	$(CPP) -c -DBUG3 MancalaBoard.cpp -o MancalaBoardB3.o

MancalaBoardB4.o : MancalaBoard.cpp
	$(CPP) -c -DBUG4 MancalaBoard.cpp -o MancalaBoardB4.o

PylosBoardB5.o : PylosBoard.cpp
	$(CPP) -c -DBUG5 PylosBoard.cpp -o PylosBoardB5.o

PylosBoardB6.o : PylosBoard.cpp
	$(CPP) -c -DBUG6 PylosBoard.cpp -o PylosBoardB6.o

PylosBoardB7.o : PylosBoard.cpp
	$(CPP) -c -DBUG7 PylosBoard.cpp -o PylosBoardB7.o

PylosBoardB8.o : PylosBoard.cpp
	$(CPP) -c -DBUG8 PylosBoard.cpp -o PylosBoardB8.o

PylosBoardB9.o : PylosBoard.cpp
	$(CPP) -c -DBUG9 PylosBoard.cpp -o PylosBoardB9.o

PylosBoardB10.o : PylosBoard.cpp
	$(CPP) -c -DBUG10 PylosBoard.cpp -o PylosBoardB10.o

PylosMoveB11.o : PylosMove.cpp
	$(CPP) -c -DBUG11 PylosMove.cpp -o PylosMoveB11.o

PylosMoveB12.o : PylosMove.cpp
	$(CPP) -c -DBUG12 PylosMove.cpp -o PylosMoveB12.o

PylosMoveB13.o : PylosMove.cpp
	$(CPP) -c -DBUG13 PylosMove.cpp -o PylosMoveB13.o

PylosMoveB14.o : PylosMove.cpp
	$(CPP) -c -DBUG14 PylosMove.cpp -o PylosMoveB14.o

PylosDlgB15.o : PylosDlg.cpp
	$(CPP) -c -DBUG15 PylosDlg.cpp -o PylosDlgB15.o

BoardTestB0 : $(GAMEOBJSB0)
	$(CPP)  $(GAMEOBJSB0) -o BoardTestB0

BoardTestB1 : $(GAMEOBJSB1)
	$(CPP)  $(GAMEOBJSB1) -o BoardTestB1

BoardTestB2 : $(GAMEOBJSB2)
	$(CPP)  $(GAMEOBJSB2) -o BoardTestB2

BoardTestB3 : $(GAMEOBJSB3)
	$(CPP)  $(GAMEOBJSB3) -o BoardTestB3

BoardTestB4 : $(GAMEOBJSB4)
	$(CPP)  $(GAMEOBJSB4) -o BoardTestB4

BoardTestB5 : $(GAMEOBJSB5)
	$(CPP)  $(GAMEOBJSB5) -o BoardTestB5

BoardTestB6 : $(GAMEOBJSB6)
	$(CPP)  $(GAMEOBJSB6) -o BoardTestB6

BoardTestB7 : $(GAMEOBJSB7)
	$(CPP)  $(GAMEOBJSB7) -o BoardTestB7

BoardTestB8 : $(GAMEOBJSB8)
	$(CPP)  $(GAMEOBJSB8) -o BoardTestB8

BoardTestB9 : $(GAMEOBJSB9)
	$(CPP)  $(GAMEOBJSB9) -o BoardTestB9

BoardTestB10 : $(GAMEOBJSB10)
	$(CPP)  $(GAMEOBJSB10) -o BoardTestB10

BoardTestB11 : $(GAMEOBJSB11)
	$(CPP)  $(GAMEOBJSB11) -o BoardTestB11

BoardTestB12 : $(GAMEOBJSB12)
	$(CPP)  $(GAMEOBJSB12) -o BoardTestB12

BoardTestB13 : $(GAMEOBJSB13)
	$(CPP)  $(GAMEOBJSB13) -o BoardTestB13

BoardTestB14 : $(GAMEOBJSB14)
	$(CPP)  $(GAMEOBJSB14) -o BoardTestB14

BoardTestB15 : $(GAMEOBJSB15)
	$(CPP)  $(GAMEOBJSB15) -o BoardTestB15

AllBugs : BoardTestB0 BoardTestB1 BoardTestB2 BoardTestB3 BoardTestB4 BoardTestB5 BoardTestB6 BoardTestB7 BoardTestB8 BoardTestB9 BoardTestB10 BoardTestB11 BoardTestB12 BoardTestB13 BoardTestB14 BoardTestB15
	mv BoardTestB* /home/grade305/prj1/testturnin/tests

# Header file interdependencies

MancalaDlg.h PylosDlg.h: Dialog.h
	touch MancalaDlg.h PylosDlg.h

MancalaView.h PylosView.h: View.h
	touch MancalaView.h PylosView.h

MancalaBoard.cpp: Class.h
	touch MancalaBoard.cpp

MancalaMove.cpp: MancalaBoard.h Class.h
	touch MancalaMove.cpp
MancalaView.cpp: MancalaBoard.h View.h Class.h
	touch MancalaView.cpp
MancalaDlg.cpp: MancalaBoard.h Dialog.h Class.h
	touch MancalaDlg.cpp

PylosBoard.cpp: Class.h
	touch PylosBoard.cpp
PylosMove.cpp: PylosView.h PylosBoard.h 
	touch PylosMove.cpp
PylosView.cpp: PylosBoard.h View.h Class.h 
	touch PylosView.cpp
PylosDlg.cpp: PylosBoard.h Dialog.h Class.h 
	touch PylosDialog.cpp

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

# DO NOT DELETE
