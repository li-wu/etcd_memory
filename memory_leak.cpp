#include "etcd/Client.hpp"
#include <future>

class DistributedLock {
public:
    DistributedLock(const std::string &lock_name,
                    uint timeout = 0);
    ~DistributedLock() noexcept;
    inline bool lock_acquired() {
      return _acquired;
    }

private:
    bool _acquired = false;
    std::string _lock_key;
    std::unique_ptr<::etcd::Client> _etcd_client;
};

DistributedLock::DistributedLock(const std::string &lock_name,
                                 uint timeout) {
  _etcd_client = std::make_unique<etcd::Client>("localhost:12379");

  try {
    if (timeout == 0) {
      etcd::Response resp = _etcd_client->lock(lock_name).get();
      if (resp.is_ok()) {
        _lock_key = resp.lock_key();
        _acquired = true;
      }
    } else {
      std::future<etcd::Response> future = std::async(std::launch::async, [&]() {
          etcd::Response resp = _etcd_client->lock(lock_name).get();
          return resp;
      });

      std::future_status status = future.wait_for(std::chrono::seconds(timeout));
      if (status == std::future_status::ready) {
        auto resp = future.get();
        if (resp.is_ok()) {
          _lock_key = resp.lock_key();
          _acquired = true;
        }
      } else if (status == std::future_status::timeout) {
        std::cout << "failed to acquire distributed because of lock timeout" << std::endl;
      } else {
        std::cout << "failed to acquire distributed lock" << std::endl;
      }
    }
  } catch (std::exception &e) {
    throw e;
  }
}

DistributedLock::~DistributedLock() noexcept {
  if (!_acquired) {
    return;
  }

  try {
    auto resp = _etcd_client->unlock(_lock_key).get();
    if (!resp.is_ok()) {
      std::cout << resp.error_code() << std::endl;
    }
  } catch (std::exception &e) {
    throw e;
  }
}

int main() {
  int i = 0;
  while(true) {
    {
      DistributedLock lock(std::to_string(i), 3);
      if(!lock.lock_acquired()) {
        std::cout << "failed to acquire lock" << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds (10));
    }
    ++i;
    if (i == 10) {
      i = 0;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
}