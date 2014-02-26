mkdir python-spi
cd python-spi
read
wget https://raw.github.com/doceme/py-spidev/master/setup.py
read
wget https://raw.github.com/doceme/py-spidev/master/spidev_module.c
read
sudo python setup.py install

