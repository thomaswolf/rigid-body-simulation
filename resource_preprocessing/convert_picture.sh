FILES=("ETHZ" "CGL_Logo-Text_Right" "trump_wall" "wall" "duck")
PERCENTAGES=("10" "1.1" "5" "5" "10")

for ((i=0;i<${#FILES[@]};++i))
do
	FILE=${FILES[i]}
	echo $FILE
	PERCENTAGE=${PERCENTAGES[i]}
	echo $PERCENTAGE
	convert $FILE.png -colorspace sRGB -filter point -flatten -resize $PERCENTAGE% -colorspace RGB $FILE.txt
	# for control
	convert $FILE.png -colorspace sRGB -filter point -flatten -resize $PERCENTAGE% -colorspace RGB $FILE.bmp
	# post processing
	sed -i 's/# ImageMagick pixel enumeration: //g' $FILE.txt
	sed -i 's/,srgba//g' $FILE.txt
	sed -i 's/: (/,/g' $FILE.txt
	sed -i 's/\#.*//' $FILE.txt
	sed -i 's/.,*)  //' $FILE.txt
	cp $FILE.txt ../resources/$FILE.txt
	rm $FILE.txt
done
