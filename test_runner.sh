#!/bin/bash

# Renk kodları
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== B-Minor Derleyicisi Derleniyor ===${NC}"
make

echo -e "\n${BLUE}=== Otomatik Testler Basliyor ===${NC}"

if [ ! -d "tests" ]; then
  mkdir tests
fi

# tests klasöründeki tüm .bminor dosyalarını bul
for test_file in tests/*.bminor; do
    [ -e "$test_file" ] || continue 
    
    echo -e "\n${BLUE}------------------------------------------------${NC}"
    echo -e "Test Edilen Dosya: ${GREEN}$test_file${NC}"
    
    # Yeni derleyicimiz bminor'u çalıştır
    ./bminor "$test_file"
done

echo -e "\n${BLUE}------------------------------------------------${NC}"
echo "Tum testler tamamlandi!"