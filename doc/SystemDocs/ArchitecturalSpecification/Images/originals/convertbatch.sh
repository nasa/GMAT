for i in ls -C1 *.png
do
convert -verbose $i $i.eps
done
