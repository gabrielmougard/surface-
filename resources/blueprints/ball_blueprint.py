import math
import time
from surfacepp import Vec3, PyEntity


class DerivedPyEntity(PyEntity):
    def __init__(self, uuid, tag, transform):
        super(DerivedPyEntity, self).__init__(uuid, tag, transform)

    def on_update(self, delta_time):
        self.transform.position.y = 10 - 5 * math.sin(self.transform.position.x * time.time() / 100)
        self.transform.position.z = -160 - 5 * math.cos(self.transform.position.x * time.time() / 30)

    def on_create(self):
        super(DerivedPyEntity, self).on_create()
        print("Entity {}:{} on create".format(self.uuid.uuid, self.tag.tag))

    def on_destroy(self):
        super(DerivedPyEntity, self).on_destroy()
        print("Entity {} on destroy".format(self.tag.tag))

