import math
import time
from surfacepp import Vec3, PyEntity


class DerivedPyEntity(PyEntity):
    def __init__(self, uuid, tag, transform):
        super(DerivedPyEntity, self).__init__(uuid, tag, transform)

    def on_update(self, delta_time):
        self.transform.position.x += math.sin(time.time())
        self.transform.position.z += math.cos(time.time())
        # self.transform.position = Vec3(1, 2, 3)
        # self.tag.tag = "Hello!"


    def on_create(self):
        super(DerivedPyEntity, self).on_create()
        print("Entity {} created".format(self.tag.tag))

    def on_destroy(self):
        super(DerivedPyEntity, self).on_destroy()
        print("Entity {} on destroy".format(self.tag.tag))

