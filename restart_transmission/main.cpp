
#include <cstdlib>
#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
  int result;
  do
    {
      result = std::system("transmission-remote --auth suro:poney1torrent2 -l");
      if (result != 0)
	{
	  std::system("sudo service transmission-daemon restart");
	  std::this_thread::sleep_for(std::chrono::seconds(600));
	}
    } while (result != 0);
  return 0;
}
