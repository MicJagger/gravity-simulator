use crate::{bodies::Bodies, camera::Camera};

/// speed of causality c in meters/second
pub const C: f64 = 299792458.0;

pub struct Universe {
    pub bodies: Bodies,
    pub camera: Camera,
}

impl Universe {
    pub fn new() -> Self {
        Universe {
            bodies: Bodies::new(),
            camera: Camera::new(),
        }
    }
}
