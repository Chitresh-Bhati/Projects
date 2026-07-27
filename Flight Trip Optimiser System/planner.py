from flight import Flight

class Planner:
    def __init__(self, flights):
        """Initialize the Flight Planner
        
        Args:
            flights (List[Flight]): List of Flight objects containing route information
        """
        # Finding number of cities using max from both start and end cities
        max_city = -1
        for flight in flights:
            if flight.end_city > max_city:
                max_city = flight.end_city
            if flight.start_city > max_city:
                max_city = flight.start_city
        self.cities = max_city + 1

        # Create adjacency list representation of the graph
        self.adj_list = [[] for _ in range(self.cities)]
        for flight in flights:
            self.adj_list[flight.start_city].append(flight)
    
    def least_flights_earliest_route(self, start_city, end_city, t1, t2):
        # Track visited states to avoid cycles
        
        visited = [ [] for _ in range(self.cities)]
        
        minheap = MinHeap([(0, t1, start_city, [], t1)], 
                         comparator=lambda x, y: x[0] < y[0] or (x[0] == y[0] and x[1] < y[1]))
        
        while not minheap.is_empty():
            num_flights, curr_time, curr_city, route, dep_limit = minheap.extract_min()
            
            state = (curr_city, dep_limit)
            if state in visited[state[0]]:
                continue
            visited[state[0]].append(state)
                
            if curr_city == end_city and curr_time <= t2:
                return route
                
            for flight in self.adj_list[curr_city]:
                if flight.departure_time >= dep_limit and flight.departure_time >= t1:
                    new_dep_limit = flight.arrival_time + 20
                    minheap.insert((
                        num_flights + 1,
                        flight.arrival_time,
                        flight.end_city,
                        route + [flight],
                        new_dep_limit
                    ))
        
        return []

    def cheapest_route(self, start_city, end_city, t1, t2):

        visited = [ [] for _ in range(self.cities)]        

        comp_function = lambda x, y: x[0] < y[0] or (x[0] == y[0] and x[1] < y[1])

        minheap = MinHeap([(0, t1, start_city, [], t1)],comp_function)
        
        while not minheap.is_empty():
            total_fare, curr_time, curr_city, route, dep_limit = minheap.extract_min()
            
            state = (curr_city, dep_limit)

            if state in visited[state[0]]:
                continue
            visited[state[0]].append(state)
            
            if curr_city == end_city and curr_time <= t2:
                return route
                
            for flight in self.adj_list[curr_city]:
                if flight.departure_time >= dep_limit and flight.departure_time >= t1:
                    new_dep_limit = flight.arrival_time + 20
                    minheap.insert((
                        total_fare + flight.fare,
                        flight.arrival_time,
                        flight.end_city,
                        route + [flight],
                        new_dep_limit
                    ))
        return []
    
    def least_flights_cheapest_route(self, start_city, end_city, t1, t2):
        # visited = set()
        visited = [[] for _ in range(self.cities)]
        
        minheap = MinHeap([(0, 0, t1, start_city, [], t1)],
                         comparator=lambda x, y: x[0] < y[0] or (x[0] == y[0] and x[1] < y[1]))
        
        while not minheap.is_empty():
            num_flights, total_fare, curr_time, curr_city, route, dep_limit = minheap.extract_min()
            
            state = (curr_city, dep_limit)

            if state in visited[state[0]]:
                continue
            visited[state[0]].append(state)
            
            if curr_city == end_city and curr_time <= t2:
                return route
                
            for flight in self.adj_list[curr_city]:
                if flight.departure_time >= dep_limit and flight.departure_time >= t1:
                    new_dep_limit = flight.arrival_time + 20
                    minheap.insert((
                        num_flights + 1,
                        total_fare + flight.fare,
                        flight.arrival_time,
                        flight.end_city,
                        route + [flight],
                        new_dep_limit
                    ))
        
        return []


class MinHeap:
    
    def __init__(self, initial_list=None, comparator=lambda x, y: x < y):
        self.heap = []
        self.comparator = comparator
        
        if initial_list:
            self.heap = list(initial_list)
            for i in range(len(self.heap) // 2 - 1, -1, -1):
                self._sift_down(i)
    
    def parent(self, i):
        return (i - 1) // 2
    
    def left_child(self, i):
        return 2 * i + 1
    
    def right_child(self, i):
        return 2 * i + 2
    
    def swap(self, i, j):
        self.heap[i], self.heap[j] = self.heap[j], self.heap[i]
    
    def insert(self, key):
        self.heap.append(key)
        self._sift_up(len(self.heap) - 1)
    
    def _sift_up(self, i):
        while i > 0:
            parent = self.parent(i)
            if self.comparator(self.heap[i], self.heap[parent]):
                self.swap(i, parent)
                i = parent
            else:
                break
    
    def extract_min(self):
        if not self.heap:
            raise IndexError("Heap is empty")
        
        if len(self.heap) == 1:
            return self.heap.pop()
        
        min_val = self.heap[0]
        self.heap[0] = self.heap.pop()
        self._sift_down(0)
        
        return min_val
    
    def _sift_down(self, i):
        while True:
            min_index = i
            left = self.left_child(i)
            right = self.right_child(i)
            
            if left < len(self.heap) and self.comparator(self.heap[left], self.heap[min_index]):
                min_index = left
            
            if right < len(self.heap) and self.comparator(self.heap[right], self.heap[min_index]):
                min_index = right
            
            if i != min_index:
                self.swap(i, min_index)
                i = min_index
            else:
                break

    def is_empty(self):
        return len(self.heap) == 0