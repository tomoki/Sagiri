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
            printf " => \033[0;32mPass\033[0;39m\n"
            rm -f $asm $obj $startupobj $exe $out
        else
            printf " => \033[0;31mFailed\033[0;39m\n"
            printf "Expected:\n"
            cat $expected
            printf "Output:\n"
            cat $out
            rm -f $asm $obj $startupobj $exe $out
        fi
        exit 0
    else
        printf " => \033[0;31mFailed because %s not found\033[0;39m\n" "$expected"
        exit 0
    fi
else
    printf " => \033[0;31mFailed because %s not found\033[0;39m\n" "$program"
    exit 0
fi
