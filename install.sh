
if [ ! -e js/jquery ]; then
    mkdir js/jquery
    cd js/jquery
    wget https://code.jquery.com/jquery-3.3.1.min.js
    cd ../..
fi
