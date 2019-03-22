
if [ ! -e js ]; then
    mkdir js
    cd js
    wget https://code.jquery.com/jquery-3.3.1.min.js
    cd ..
fi
