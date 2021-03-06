//
//  main.cpp
//  Peeps
//
//  Created by William Wayland on 9/10/17.
//  Copyright © 2017 William Wayland. All rights reserved.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <array>
#include <string>
#include <condition_variable>

class Peep
{
public:
  Peep(
       int left, int right, const std::string& name,
       std::array<std::mutex, 5>& forks,
       std::mutex& write_guard,
       int forks_till_full
       ) :
  left_(left),
  right_(right),
  name_(name),
  forks_(forks),
  write_guard_(write_guard),
  forks_till_full_(forks_till_full)
  {
  }
  
  // Eat away kiddo
  void eat()
  {
    for (int i = 0; i < forks_till_full_; i++)
    {
      std::unique_lock<std::mutex> guard_r(forks_[right_], std::defer_lock);
      std::unique_lock<std::mutex> guard_l(forks_[left_], std::defer_lock);
      std::lock(guard_r, guard_l);
      
      write_guard_.lock();
      std::cout << name_ << " has started eating..." << std::endl;
      write_guard_.unlock();
      
      // Eating.. yum!
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      
      write_guard_.lock();
      std::cout << name_ << " has finished eating..." << std::endl;
      write_guard_.unlock();
    }
  }
  
  // Spawns a thread for this object to eat...
  std::thread spawn()
  {
    return std::thread(&Peep::eat, this);
  }
  
private:
  int left_, right_;
  const std::string& name_;
  std::array<std::mutex, 5>& forks_;
  std::mutex& write_guard_;
  int forks_till_full_;
};

int main() {
  std::array<std::mutex, 5> forks;
  std::mutex write_guard;
  
  const std::array<std::string, 5> names =
  { "Karl Marx", "Lenin", "Fidel Castro", "Xi Jinping", "Leon Trotsky" };
  
  const int forks_till_fill = 2;
  std::array<Peep, 5> peeps =
  {
    Peep(0, 1, names[0], forks, write_guard, forks_till_fill),
    Peep(1, 2, names[1], forks, write_guard, forks_till_fill),
    Peep(2, 3, names[2], forks, write_guard, forks_till_fill),
    Peep(3, 4, names[3], forks, write_guard, forks_till_fill),
    Peep(4, 0, names[4], forks, write_guard, forks_till_fill)
  };
  
  std::vector<std::thread> chairs;
  for (auto& peep : peeps)
  {
    chairs.push_back(peep.spawn());
  }
  
  for (auto& chair : chairs)
  {
    chair.join();
  }
  
  std::cout << "Finished..." << std::endl;
  
  return 0;
}
