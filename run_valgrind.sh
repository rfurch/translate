
echo " Testing Memory leaks..."

valgrind --leak-check=full ./translateExample01 -i "I don't like this color" -vvv -f en -t es


