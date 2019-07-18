#!/bin/bash
####################################################################
#depsym.sh ios 2014 Vojtech Sikula xsikul13(at)stud.fit.vutbr.cz (c)#
###################################################################
gopt="false"
ropt="false"
dopt="false"
export i=1
###########################################################
#zpracování parametrù 

OBJECT_ID=""

while getopts 'gr:d:' opt; do
  case "$opt" in
    g)
      gopt=true ;;
    r)
      ropt=true
      OBJECT_ID=${OPTARG##*/} ;;
    d)
      dopt=true
      OBJECT_ID=${OPTARG##*/} ;;
    *)
      exit 1 ;;
  esac
done
[ $dopt = "true" ] && [ $ropt = "true" ] && exit 1

#################################################################
UZLY=""
UZLYs=""
predchozi="false"
for arg in $@ ; do
	if [ -f $arg ]; then
		
		if [ $predchozi = "false" ]; then 
			predchozi="true"
			[ "$ropt" != "$dopt" ] && continue
		fi 

		if [ "$gopt" = "true" ]; then
			dsym="${dsym}
${arg##*/}!o!
"
			usym="${usym}
${arg##*/}!o!
"
		else
			dsym="$dsym
$arg!o!
"
			usym="$usym
$arg!o!
"
		fi
	UZLYs="$UZLYs$arg
"	
	UZLY="$UZLY$arg
"
		
		dsym="$dsym"`nm $arg | grep "[^@]" | egrep "( D | T | B | C | G )" | sed 's/^[^BCDGT]*[BCDGT] //g'`
		usym="$usym"`nm $arg | grep "[^@]" | egrep "( U )" | sed 's/^[^U]*[U] //g'`
		((i++))		
	fi 
done
#vsazeni obj2 (sym)

obj2=""
for arg in $dsym; do 
	pom=${arg##*/}
	if [ "$arg" = "${arg%!o!}" ]; then
		usym=`echo "$usym" | sed "s:^$arg$:!S! $obj2 ($arg):g"`
	else
		obj2="$arg"
	fi
done

obj1=""
us=""
IFS="
"
#predrazeni obj1
for arg in $usym; do
	if [ "$arg" = "${arg%!o!}" ]; then
		us="$us$obj1 $arg
"
	else
		obj1=$arg
	fi
done
usym=$us
pata=""


if [ "$gopt" = "true" ]; then 
	
pom=""
	[ "$ropt" = "true" ] && usym=`echo "$usym" | grep " $OBJECT_ID!o! (" | sort | uniq`
	[ "$dopt" = "true" ] && usym=`echo "$usym" | grep "^$OBJECT_ID!o! !S!" | sort | uniq`

	for arg in $UZLY; do

		pom="${arg##*/}"
		usym=`echo "$usym" | sed "s:$arg:${arg##/}:g"`
		
		pom=`echo "$pom" | sed "s/-/_/g" | sed "s/\./D/g" | sed "s/+/P/g"`
		nic=`echo "$usym" | egrep "(^${arg##*/}!o!| ${arg##*/}!o!)"`
		[ $? -ne 0 ] && continue
		pata="$pata$pom [label=\"${arg##*/}\"];
"
	done
		
	pata=`echo "$pata" | sed -e "/^$/d" | sort | uniq`
	usym=`echo "$usym" | sed "s/-/_/g" | sed "s/\./D/g" | sed "s/+/P/g"`
	usym=`echo "$usym" | grep "!S!" | sed 's/!S!/->/g' | sed 's/!o!//g' | sed -e "/^ *$/d" | sed 's/(/[label="/g' | sed 's/)/"];/g' |  sort | uniq`

	usym="digraph GSYM {
$usym
$pata
}"
	echo "$usym" | sed -e "/^ *$/d"
	exit 0
fi
usym=`echo "$usym" | grep "!S!" | sed 's/!S!/->/g' | sed 's/!o!//g' | sed -e "/^$/d" | sort | uniq`
[ "$ropt" = "true" ] && usym=`echo "$usym" | egrep "( $OBJECT_ID \(|./$OBJECT_ID \()" | sort | uniq`
[ "$dopt" = "true" ] && usym=`echo "$usym" | egrep "(^$OBJECT_ID|./$OBJECT_ID) ->" | sort | uniq`


echo "$usym"
exit 0	