import math
import time
from surfacepp import Vec3, PyEntity, PyScene


class DerivedPyScene(PyScene):
    def __init__(self, obj):
        super(DerivedPyScene, self).__init__(obj)

    def distance(self, p1, p2):
        sum = (p1.x - p2.x)**2 + (p1.y - p2.y)**2 + (p1.z - p2.z)**2
        return math.sqrt(sum)

    def on_update(self, delta_time):
        cyborg_position = Vec3(0, 0, 0)
        cube_position = Vec3(0, 0, 0)
        for e in self.entities:
            if e.tag.tag == "Cyborg":
                cyborg_position = Vec3(e.transform.position.x, e.transform.position.y, e.transform.position.z)
            elif e.tag.tag == "Cube":
                cube_position = Vec3(e.transform.position.x, e.transform.position.y, e.transform.position.z)
        # print("Cyborg position = {} {} {}, Cube position = {} {} {} Distance = {}".format(
        #     cyborg_position.x, cyborg_position.y, cyborg_position.z, cube_position.x, cube_position.y, cube_position.z, self.distance(cyborg_position, cube_position)))
        # print("Distance = {}".format(self.distance(cyborg_position, cube_position)))

    def on_create(self):
        print("Scene created")

    def on_destroy(self):
        print("Scene destroyed")

