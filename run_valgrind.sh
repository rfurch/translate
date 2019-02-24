
echo " Testing Memory leaks..."

valgrind --leak-check=full ./translateWithLocalCache -i "Another one to the local cache" -vvv -f en -t es


