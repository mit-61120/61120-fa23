#!/bin/bash
# >>> ./run-tests.sh ../../reference/vm/interpreter

ROOT=".."
TIMEOUT=60
MEM=8

INTERPRETER=$1

run_interp_tests(){
    TOTAL=0
    COUNT=0

    for filename in $ROOT/$1/$2/*.mit; do
        if ! [[ $filename =~ $ROOT/$1/$2/bad[0-9]*.mit ]]; then
            echo "RUNNING $(basename $filename)"
            rm -f tmp.out tmp.err
            if test -f $filename.memlimit; then
                MEMLIMIT="$(cat $filename.memlimit)"
            else
                MEMLIMIT=$MEM
            fi
            LIMIT="$((($MEMLIMIT + 10) * 1000))"
            if test -f $filename.in; then
                STATS=$(timeout $TIMEOUT time -v $INTERPRETER -mem $MEMLIMIT --opt=all -s $filename < $filename.in > tmp.out 2> tmp.err)
            else
                STATS=$(timeout $TIMEOUT time -v $INTERPRETER -mem $MEMLIMIT --opt=all -s $filename > tmp.out 2> tmp.err)
            fi
            CODE=$?
            USAGE=$(echo "$STATS" | grep Max | awk '{print $6}')
            if [[ $(cat $ROOT/$1/$2/$(basename $filename).out) =~ [A-Z][A-Za-z]+Exception ]]; then
                # Used to be *"${BASH_REMATCH[0]}"*, but this was relaxed
                if [[ $(cat tmp.err tmp.out) == *"Exception"* ]]; then
                    if [[ $USAGE -gt $LIMIT ]]; then
                        echo "Fail: $(basename $filename) ($USAGE > $LIMIT)"
                    else
                        COUNT=$((COUNT+1))
                    fi
                else
                    echo "Fail Exception: $(basename $filename) (exit code $CODE)"
                fi
            else
              if [[ $CODE = 0 ]]; then
                  if diff tmp.out $ROOT/$1/$2/$(basename $filename).out; then
                      if [[ $USAGE -gt $LIMIT ]]; then
                          echo "Fail: $(basename $filename) ($USAGE > $LIMIT)"
                      else
                          COUNT=$((COUNT+1))
                      fi
                  else
                      echo "Fail: $(basename $filename) (exit code $CODE)"
                  fi
              else
                echo "Fail: $(basename $filename) (exit code $CODE)"
              fi
            fi
            TOTAL=$((TOTAL+1))
        fi
        rm -f tmp.out tmp.err
    done
}

run_interp_tests a2 public
echo "Passed $COUNT out of $TOTAL tests when testing just interpreter on public a2"
run_interp_tests a2 private
echo "Passed $COUNT out of $TOTAL tests when testing just interpreter on private a2"
run_interp_tests a3 public/mit
echo "Passed $COUNT out of $TOTAL tests when testing just interpreter on public a3"
run_interp_tests a3 private
echo "Passed $COUNT out of $TOTAL tests when testing just interpreter on private a3"

run_gc_tests(){
    TOTAL=0
    COUNT=0

    LIMIT="$((($MEM + 10) * 1000))"

    for filename in $ROOT/a4/$1/garbagetest*.mit; do
        echo "RUNNING $(basename $filename)"
        rm -f tmp.out
        USAGE=$(timeout $TIMEOUT time -v $INTERPRETER -mem $MEM --opt=all -s $filename 2>&1 > tmp.out | grep Max | awk '{print $6}')
        CODE=$?
        if [[ $CODE = 0 ]]; then
            if diff tmp.out $filename.out; then
                if [[ $USAGE -gt $LIMIT ]]; then
                    echo "Fail: $(basename $filename) ($USAGE > $LIMIT)"
                else
                    COUNT=$((COUNT+1))
                fi
            else
                echo "Fail: $(basename $filename) (exit code $CODE)"
            fi
        else
            echo "Fail: $(basename $filename) (exit code $CODE)"
        fi
        TOTAL=$((TOTAL+1))
        rm -f tmp.out
    done
    echo "Passed $COUNT out of $TOTAL gc tests"
}

run_mem_tests(){
    TOTAL=0
    COUNT=0

    run_and_parse_mem() {
        echo "RUNNING $1"
        TOTAL=$((TOTAL+1))

        LIMIT="$((($2 + 10) * 1000))"
        PREFIX="$ROOT/a4/private/$1"
        rm -f tmp.out
        USAGE=$(timeout $TIMEOUT time -v $INTERPRETER -mem $2 --opt=all -s $PREFIX.mit < $PREFIX.input 2>&1 > tmp.out | grep Max | awk '{print $6}')
        if diff tmp.out $PREFIX.mit.out; then
            if [[ $USAGE -gt $LIMIT ]]; then
                echo "Fail: $1 ($USAGE > $LIMIT)"
            else
                COUNT=$((COUNT+1))
            fi
        else
            echo "Fail: $1 ($USAGE < $LIMIT)"
        fi
        rm -f tmp.out
    }

    run_and_parse_mem refcollect 4

    echo "Passed $COUNT out of $TOTAL mem tests"
}

run_gc_tests public
run_gc_tests private
run_mem_tests
echo "Done"
