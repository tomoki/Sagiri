compiler=$1
program=$2
startup=$3
expected=$2.expected
time=2s

echo "$program"
if [ -e $program ]
then
    if [ -e $expected ]
    then
        asm=`mktemp --suffix .s`
        obj=`mktemp --suffix .o`
        startupobj=`mktemp --suffix .o`
        exe=`mktemp`
        out=`mktemp`
        $compiler $program > $asm
        gcc $asm -c -o $obj
        gcc $startup -c -o $startupobj
        gcc $obj $startupobj -o $exe
        $exe > $out
        d=`diff $expected $out -Z`
        if [ ${#d} -eq 0 ]
        then
            echo -e " => \033[0;32mPass\033[0;39m"
            rm -f $asm $obj $startupobj $exe $out
        else
            echo -e " => \033[0;31mFailed\033[0;39m"
            echo "Expected:"
            cat $expected
            echo "Output:"
            cat $out
            rm -f $asm $obj $startupobj $exe $out
        fi
        exit 0
    else
        echo -e " => \033[0;31mFailed <- $expected not found\033[0;39m"
        exit 0
    fi
else
    echo -e " => \033[0;31mFailed <- $program not found\033[0;39m"
    exit 0
fi
