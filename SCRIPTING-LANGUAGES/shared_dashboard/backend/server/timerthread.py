from threading import Thread, Lock, Condition
import time
import heapq


class TimerThread(Thread):
    """Manages refresh timers for all components"""

    def __init__(self):
        super().__init__(daemon=True)
        self._timer_heap = []
        self._timer_lock = Lock()
        self._condition = Condition(self._timer_lock)
        self._running = True

    def add_timer(self, component):
        for _, comp in self._timer_heap:
            if comp.id == component.id:
                return
        if component.refresh_interval <= 0:
            return

        next_refresh = time.time() + component.refresh_interval
        with self._timer_lock:
            heapq.heappush(self._timer_heap, (next_refresh, component))
            self._condition.notify()

    def remove_timer(self, component):
        with self._timer_lock:
            self._timer_heap = [(t, c) for t, c in self._timer_heap if c != component]
            heapq.heapify(self._timer_heap)
            self._condition.notify()

    def run(self):
        while self._running:
            with self._timer_lock:
                while self._timer_heap:
                    next_time, component = self._timer_heap[0]
                    now = time.time()
                    if next_time <= now:
                        heapq.heappop(self._timer_heap)
                        try:
                            component.refresh()
                            if component.refresh_interval > 0:
                                heapq.heappush(self._timer_heap,
                                               (now + component.refresh_interval, component))
                        except Exception as e:
                            print(f"Error refreshing component: {e}")
                    else:
                        self._condition.wait(timeout=next_time - now)
                        continue

                self._condition.wait(timeout=1.0)
                time.sleep(1)

    def stop(self):
        self._running = False
        with self._timer_lock:
            self._condition.notify()
