use crate::universe;

pub struct Camera {
    /// x, y, z in meters
    position: (f64, f64, f64),
    /// in meters/second
    speed: f64,
    /// theta, phi, psi in degrees
    rotation: (f32, f32, f32),
    /// conversion from mouse displacement to _
    sensitivity: f32,
}

impl Camera {
    pub fn new() -> Self {
        Camera {
            position: (0.0, 0.0, 0.0),
            speed: universe::C * 64.0,
            rotation: (0.0, 0.0, 0.0),
            sensitivity: 1.0,
        }
    }
}
