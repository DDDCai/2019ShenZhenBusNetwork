import ctypes

so = ctypes.cdll.LoadLibrary("./libcreate_net.so")

#   Create the subway net. It must be called before all.
#   Being called one time is enough.
def create_net():
    so.produce_net_from_files()

#   (float).
def average_degree():
    so.average_degree.restype = ctypes.c_float
    return so.average_degree()

#   Get the degree distribution.
#   It will return an array (int) marked as tmp.
#   So tmp[0] means the maxmuim degree of the graph.
#   And tmp[i] (0 < i <= tmp[0]) means the number of nodes whose degrees are i is tmp[i].
def degree_distribution():
    so.degree_distribution.restype = ctypes.POINTER(ctypes.c_int)
    return so.degree_distribution()

#   (float).
def average_clustering_c():
    so.average_c.restype = ctypes.c_float
    return so.average_c()

#   Get the adjacent matrix of the net.
def transfor_table_to_matrix():
    so.transfor_table_to_matrix_with_weight.restype = ctypes.POINTER(ctypes.POINTER(ctypes.c_int))
    return so.transfor_table_to_matrix_with_weight()

#   Get the average shortest path (float).
def average_shortest_path():
    so.average_shortest_path.restype = ctypes.c_float
    return so.average_shortest_path()

#   Get the coreness of the net (int).
def coreness_of_a_graph():
    so.coreness_of_a_graph.restype = ctypes.c_int
    return so.coreness_of_a_graph()

#   Get the (int)degree of a station using its (string)name.
def degree_of_a_station(name):
    so.degree_of_a_station.restype = ctypes.c_int
    name_ = name + '\r\n'
    name__ = bytes(name_,encoding='utf-8')
    return so.degree_of_a_station(name__)

#   Get the (float)clustering coefficient of a station using its (string)name.
def c_of_a_station(name):
    so.c_of_a_station.restype = ctypes.c_float
    name_ = name + '\r\n'
    name__ = bytes(name_,encoding='utf-8')
    return so.c_of_a_station(name__)

#   Get the (int)coreness of a station using its (string)name.
def coreness_of_a_station(name):
    so.coreness_of_a_station.restype = ctypes.c_int
    name_ = name + '\r\n'
    name__ = bytes(name_,encoding='utf-8')
    return so.coreness_of_a_station(name__)

#   Get the (int)id using (string)name.
def transfor_name_to_id(name):
    so.transfor_name_to_id.restype = ctypes.c_int
    name_ = name + '\r\n'
    # name_ = name
    name__ = bytes(name_,encoding='utf-8')
    return so.transfor_name_to_id(name__)

#   Get the (string)name using (int)id.
def transfor_id_to_name(id):
    so.transfor_id_to_name.restype = ctypes.POINTER(ctypes.c_char)
    name_ = so.transfor_id_to_name(id)
    name = ctypes.string_at(name_,-1)
    return name.decode('utf-8')

#   It must be called at the end of program.
def free_memory():
    so.free_memory()

#   After called degree_distribution(), it must be called to free the given C pointer.
def c_free(pointer):
    so.c_free(pointer)

#   Get the total number of nodes (int).
def get_node_num():
    so.get_node_num.restype = ctypes.c_int
    return so.get_node_num()

#   Randomly attack percent% nodes.
def random_attack(percent):
    so.random_attack(percent)

#   Attack the percent% nodes who have the biggest degrees.
def intention_attack(percent):
    so.intention_attack(percent)

#   Get the number of sub_graphs.
def num_of_sub_graph():
    so.num_of_sub_graph.restype = ctypes.POINTER(ctypes.c_int)
    return so.num_of_sub_graph()

def find_a_path_array(from_name,to_name):
    so.find_a_path.restype = ctypes.POINTER(ctypes.c_int)
    from_name_ = from_name + '\r\n'
    from_name__ = bytes(from_name_,encoding='utf-8')
    to_name_ = to_name + '\r\n'
    to_name__ = bytes(to_name_,encoding='utf-8')
    return so.find_a_path(from_name__,to_name__)



create_net()


# intention_attack(10)


# print('node_num:',get_node_num())

# d_distribution = degree_distribution()
# for i in range(d_distribution[0] + 1):
#     if i==0 :
#         continue
#     if d_distribution[i]==0 :
#         continue
#     print('The number of nodes whose degrees are ',i,' is ',d_distribution[i])
# c_free(d_distribution)

# print('average degree:',average_degree())
# print('average coefficient:',average_clustering_c())
# # print('average shortest path:',average_shortest_path())
a = num_of_sub_graph()
print('number of sub-graph:',a[0])
# print('coreness:',coreness_of_a_graph())

# random_attack(1)
# intention_attack(1)
# print('node_num:',get_node_num())

# d_distribution_ = degree_distribution()
# for i in range(d_distribution_[0] + 1):
#     if i==0 :
#         continue
#     if d_distribution_[i]==0 :
#         continue
#     print('The number of nodes whose degrees are ',i,' is ',d_distribution_[i])
# c_free(d_distribution_)

# print('average degree:',average_degree())
# print('average coefficient:',average_clustering_c())
# print('average shortest path:',average_shortest_path())
# print('number of sub-graph:',num_of_sub_graph())
# print('coreness:',coreness_of_a_graph())


free_memory()