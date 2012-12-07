CLINTSEXEC="../milestone1/clintsExec"

OTHELLO="Othello"
PYLOS="Pylos"
CHECKERS="Checkers"
BOARD="Board"

### Input directories ###
MILESTONE0DIR="./"



for game in $OTHELLO $PYLOS $CHECKERS
do
   for i in 0 1 2 3 4 5
      do
         echo $game" Test "$i

         input=$MILESTONE0DIR$game"."$i

         # Execute myexec
         time "./"$CLINTSEXEC $game$BOARD < $input > /dev/null

      done
done




