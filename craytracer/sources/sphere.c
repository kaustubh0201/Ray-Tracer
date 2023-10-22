#include "sphere.h"
#include <stdalign.h>
#include <tgmath.h>
#include "util.h"

static bool hit(
        const Object * restrict obj, 
        Ray r, 
        CFLOAT t_min, 
        CFLOAT t_max, HitRecord * outRecord )
{
    if(obj->objType == SPHERE){
        return obj_sphereHit((const Sphere *)obj->object, r, t_min, t_max, outRecord);
    }else if(obj->objType == OBJLL){
        return obj_objLLHit((const ObjectLL *)obj->object, r, t_min, t_max, outRecord);
    }


    return false;
}


bool obj_sphereHit(const Sphere* restrict s, Ray r, CFLOAT t_min, CFLOAT t_max, HitRecord * outRecord){
    vec3 oc = r.origin;
    vec3 direction = r.direction;

    /* 
     * center - center of the sphere
     * radius - radius of the sphere
     * direction - direction of the ray
     */

    // oc = origin - center
    vector3_subtract(&oc, &s->center);

    // a = dot(direction, direction)
    CFLOAT a = vector3_dot_product(&direction, &direction);

    // half_b = dot((origin - center), direction)
    CFLOAT half_b = vector3_dot_product(&oc, &direction);
    
    // c = dot(origin, origin) - radius^2
    CFLOAT c = vector3_dot_product(&oc, &oc) - s->radius * s->radius;

    // discri = half_b^2 - a * c
    CFLOAT discri = half_b * half_b - a*c;

    // If the discriminant is less than 0 then no intersection
    if(discri < 0){
        // outRecord->valid = false;
        return false;
    }

    // sqrtd = sqrt(discri)
    CFLOAT sqrtd = sqrt(discri);

    // root1 = (-half_b - sqrtd) / a
    CFLOAT root = (-half_b - sqrtd) / a;

    // If the intersection point corresponding to this root is 
    // not in the intersection range then check the other point    
    if(root < t_min || t_max < root){
        root = (-half_b + sqrtd) / a;

        // If neither roots correspond to an intersection point in
        // the intersection range then return invalid
        if(root < t_min || t_max < root){
            // outRecord->valid = false; 
            return false;
        }
    }

    // t = root
    CFLOAT t = root;

    // inter1 = direction
    vec3 inter1 = direction;

    // inter1 = root * direction
    vector3_multiplyf(&inter1, root);

    // p = inter1 + origin
    vec3 p = *(vector3_add(&inter1, &r.origin));
    // n = p
    vec3 n = p;
    
    // n = p - center
    vector3_subtract(&n, &s->center);
    
    // n = (p - center)/radius
    vector3_multiplyf(&n, 1/s->radius);
    
    hr_setRecordi(t, p, n, direction, outRecord, &s->sphMat);
    obj_sphereTexCoords(outRecord->normal, &outRecord->u, &outRecord->v);

    return true;
}

void obj_sphereTexCoords(vec3 pointOnSphere,
                         CFLOAT * outU, CFLOAT * outV){
    CFLOAT theta = acos(-pointOnSphere.y); 
    CFLOAT phi = atan2(-pointOnSphere.z, pointOnSphere.x) + CF_PI; 

    *outU = phi / (2 * CF_PI);
    *outV = theta / CF_PI;

}

Object * obj_createObject(void * restrict object, 
                          ObjectType type, 
                          DynamicStackAlloc * restrict dsa)
{
    Object * o = alloc_dynamicStackAllocAllocate(dsa, sizeof(Object), alignof(Object));
    
    o->object = object;
    o->objType = type;
    return o;
} 

bool obj_objectLLAdd(
        ObjectLL * restrict objll, 
        void * restrict obj, 
        ObjectType objType
)
{
    if(!objll || !objll->valid){
        return false;
    }

    ObjectLLNode * olln = alloc_dynamicStackAllocAllocate(objll->dsa, 
                          sizeof(ObjectLLNode), alignof(ObjectLLNode)); 
    
    olln->obj.object = obj;
    olln->obj.objType = objType;

    if(!objll->head){
        olln->next = NULL;
    }else{
        olln->next = objll->head;
    }

    objll->head = olln;
    objll->numObjects += 1;

    return true;
}

bool obj_objectLLRemove(ObjectLL * restrict objll, size_t index){
    if(!objll || !objll->valid){
        return false;
    }
    
    if(objll->numObjects <= index){
        return false;
    }
    
    if(index == 0){
        objll->head = objll->head->next;
        return true;  
    }

    size_t i = 0;
    ObjectLLNode * cur = objll->head;
    while(i < index){
        cur = cur->next;
        i++;
    }
    
    cur->next = cur->next->next;
    objll->numObjects -= 1;

    return true;
}

/*HitRecord**/
bool obj_objLLHit (const ObjectLL* restrict objll,
                   Ray r, 
                   CFLOAT t_min,
                   CFLOAT t_max,
                   HitRecord * out){
    
    if(!objll || !objll->valid){
        return false; //NULL;
    }

    HitRecord * hr = (HitRecord *) alloc_linearAllocFCAllocate(objll->hrAlloc);
    HitRecord * h = NULL;

    ObjectLLNode * cur = objll->head;
    while(cur != NULL){
        hit(&cur->obj, r, t_min, t_max, hr);
        
        if(hr->valid){
            if(h == NULL ){
                h = hr;
                hr = (HitRecord *) alloc_linearAllocFCAllocate(objll->hrAlloc);
            }else if(hr->distanceFromOrigin < h->distanceFromOrigin){
                h = hr;
                hr = (HitRecord *) alloc_linearAllocFCAllocate(objll->hrAlloc);
            }

        }

        cur = cur->next;
    }
    
    if(h != NULL){
        *out = *h;  
        return true;
    }

    return false;
    //return h;
}


bool obj_objLLAddSphere(ObjectLL * restrict objll,
        Sphere sphere)
{
    Sphere * s = alloc_dynamicStackAllocAllocate(objll->dsa, sizeof(Sphere), alignof(Sphere));
    *s = sphere; 
    if(!s) { return false; }

    return obj_objectLLAdd(objll, (void *) s, SPHERE);
}

ObjectLL * obj_createObjectLL(    
    DynamicStackAlloc * dsaAlloc, 
    DynamicStackAlloc * dsaObjs
){
    
    ObjectLL * objLL = alloc_dynamicStackAllocAllocate(dsaAlloc, sizeof(ObjectLL), alignof(ObjectLL));

    objLL->numObjects = 0;
    objLL->head = NULL;
    objLL->valid = true;
    objLL->dsa = dsaObjs;
    objLL->hrAlloc = NULL; 

    return objLL;
}

Object * obj_objectLLGetAT(const ObjectLL * restrict objll, size_t index){
    if(index >= objll->numObjects){
        return NULL; 
    }

    ObjectLLNode * cur = objll->head;
    size_t i = 0;
    while(i < index){
        cur = cur->next;
        i += 1;
    }
    
    return &cur->obj;
}


void obj_objectLLSetAT(const ObjectLL * restrict objll, size_t index, Object object){
    if(index >= objll->numObjects){
        return; 
    }

    ObjectLLNode * cur = objll->head;
    size_t i = 0;
    while(i < index){
        cur = cur->next;
        i += 1;
    }
    
    cur->obj = object; 
}

void obj_objectLLSort(const ObjectLL * restrict objll, 
                      size_t start, 
                      size_t end, ObjectComparator comp){

    if(start >= end || end > objll->numObjects || start > objll->numObjects){
        return;
    }

    size_t i = start + 1;

    while(i < (end + 1)){
        Object * key = obj_objectLLGetAT(objll, i);
        size_t j = i - 1;

        Object * aj = obj_objectLLGetAT(objll, j);
        while(j != ((size_t)-1) && comp(aj, key)){
            obj_objectLLSetAT(objll, j + 1, *aj); 
            j -= 1;
            aj = obj_objectLLGetAT(objll, j);
        }
        obj_objectLLSetAT(objll, j + 1, *key); 
        i += 1;
    }
}

bool obj_AABBHit(const AABB* restrict s, Ray r, CFLOAT t_min, CFLOAT t_max){
    for(int i = 0; i < 3; i++){
        CFLOAT t0 = CF_MIN(
            (s->minimum.v[i] - r.origin.v[i])/(r.direction.v[i]),
            (s->maximum.v[i] - r.origin.v[i])/(r.direction.v[i])
        );

        CFLOAT t1 = CF_MAX(
            (s->minimum.v[i] - r.origin.v[i]) / r.direction.v[i],
            (s->maximum.v[i] - r.origin.v[i]) / r.direction.v[i]
        );

        t_min = CF_MAX(t0, t_min);
        t_max = CF_MIN(t1, t_max);

        if(t_max<=t_min){
            return false;
        }
    }
    return true;
}

bool obj_sphereCalcBoundingBox(const Sphere* restrict s, AABB * outbox){
    outbox->minimum.x = s->center.x - s->radius;
    outbox->minimum.y = s->center.y - s->radius;
    outbox->minimum.z = s->center.z - s->radius;

    outbox->maximum.x = s->center.x + s->radius;
    outbox->maximum.y = s->center.y + s->radius;
    outbox->maximum.z = s->center.z + s->radius;
    
    return true;
}

static AABB surrounding_box(const AABB* restrict box0, const AABB* restrict box1){
    vec3 small;
    vec3 big;
    AABB temp_AABB;

    small.x = CF_MIN(box0->minimum.x, box1->minimum.x);
    small.y = CF_MIN(box0->minimum.y, box1->minimum.y);
    small.z = CF_MIN(box0->minimum.z, box1->minimum.z);
    
    big.x = CF_MAX(box0->maximum.x, box1->maximum.x);
    big.y = CF_MAX(box0->maximum.y, box1->maximum.y);
    big.z = CF_MAX(box0->maximum.z, box1->maximum.z);

    temp_AABB.minimum = small;
    temp_AABB.maximum = big;
    return temp_AABB;
    
}


static bool boundingBox(const Object * restrict obj, AABB* outbox){
    if(obj->objType == SPHERE){
        return obj_sphereCalcBoundingBox(((const Sphere *)obj->object), outbox);
    }else if(obj->objType == OBJLL){
        return obj_objectLLCalcBoundingBox((const ObjectLL *)obj->object, outbox);
    }else if(obj->objType == OBJBVH){ 
        return obj_bvhCalcBoundingBox((const BVH *)obj->object, outbox); 
    }
    
    return false;
}

bool obj_objectLLCalcBoundingBox(const ObjectLL* restrict objll, AABB * outbox){
    if(objll->numObjects == 0) return false;
    
    AABB tempBox;
    bool firstBox = true;

    ObjectLLNode* cur = objll->head;

    while(cur != NULL){
        
        if(!boundingBox(&cur->obj, &tempBox)) return false;

        
        if(firstBox){
            *outbox = tempBox;
        }else{
            *outbox = surrounding_box(outbox, &tempBox);
        }
        
        firstBox = false;
        cur = cur->next;
    }    

    return true;
}


bool obj_bvhCalcBoundingBox(const BVH * restrict bvh, AABB * restrict outbox){
    *outbox = bvh->box;
    return true;
}

bool obj_bvhHit(const BVH* restrict bvh, Ray r, CFLOAT t_min, CFLOAT t_max, HitRecord * out){
    if(!obj_AABBHit(&bvh->box, r, t_min, t_max)){
        return false;
    }

    HitRecord rec;
    rec.valid = false;

    bool hitLeft = hit(bvh->left, r, t_min, t_max, &rec);
    bool hitRight = hit(bvh->right, r, t_min, hitLeft ? rec.distanceFromOrigin : t_max, &rec);

    *out = rec;
    
    return hitLeft || hitRight;
}

static bool boxCompare(const Object * obj1, const Object * obj2, int axis){
    AABB a;
    AABB b;

    if(!boundingBox(obj1, &a) || !boundingBox(obj2, &b)){
        printf("wtf \n");  
    }

    return a.minimum.v[axis] < b.minimum.v[axis];
}

static bool boxXCompare(const Object * obj1, const Object * obj2){
    return boxCompare(obj1, obj2, 0);
} 

static bool boxYCompare(const Object * obj1, const Object * obj2){
    return boxCompare(obj1, obj2, 1);
}

static bool boxZCompare(const Object * obj1, const Object * obj2){
    return boxCompare(obj1, obj2, 2);
}

void obj_fillBVH(BVH * restrict bvh, 
                   const ObjectLL * restrict objects,
                   size_t start, size_t end){
    
    uint32_t axis = util_randomRange(0, 2); 
    size_t objectSpan = end - start;

    ObjectComparator comp = NULL;

    if(axis == 0) { comp = boxXCompare; }
    else if(axis == 1){ comp = boxYCompare; }
    else { comp = boxZCompare; }

    if(objectSpan == 1){
        bvh->left = bvh->right = obj_objectLLGetAT(objects, start); 
    }else if(objectSpan == 2){
        Object * o = obj_objectLLGetAT(objects, start);
        Object * o2 = obj_objectLLGetAT(objects, start + 1); 

        if(comp(o, o2)){
            bvh->left = o;
            bvh->right = o2;
        }else{
            bvh->left = o2;
            bvh->right = o; 
        }
    }else {
        obj_objectLLSort(objects, start, end, comp); 

        size_t mid = start + objectSpan / 2;
        BVH * left  = obj_createBVH(bvh->dsa, bvh->dsa); 
        BVH * right = obj_createBVH(bvh->dsa, bvh->dsa); 

        obj_fillBVH(left, objects, start, mid);
        obj_fillBVH(right, objects, mid, end);

        bvh->left  = obj_createObject(left, OBJBVH, bvh->dsa);
        bvh->right = obj_createObject(right, OBJBVH, bvh->dsa);
    }

    AABB leftBox, rightBox;
        
    if(!boundingBox(bvh->left, &leftBox) || !boundingBox(bvh->right, &rightBox)){
        printf("wtf2 \n");
    }
    
    bvh->box = surrounding_box(&leftBox, &rightBox);
}


BVH * obj_createBVH(DynamicStackAlloc * alloc, DynamicStackAlloc * dsa){
    BVH * bvh = alloc_dynamicStackAllocAllocate(alloc, sizeof(BVH), alignof(BVH));
    
    bvh->right = bvh->left = NULL;
    bvh->dsa = dsa;

    return bvh;
}
