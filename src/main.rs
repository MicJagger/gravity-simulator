mod bodies;
mod camera;
mod state;
mod universe;
mod vertices;
mod window;

use std::{
    error::Error,
    time::{Duration, Instant},
};

use sdl3::event::Event;

use crate::{bodies::Body, universe::Universe, vertices::Vertices, window::Window};

fn main() -> Result<(), Box<dyn Error>> {
    env_logger::init();
    let window_size = (800u32, 600u32);
    let mut window = Window::new(window_size);
    let mut universe = Universe::new();

    let name = String::from("test body");
    let body = Body {
        name: name.clone(),
        position: (0.0, 0.0, 0.0),
        velocity: (0.0, 0.0, 0.0),
        rotation: (0.0, 0.0, 0.0),
        rotation_velocity: (0.0, 0.0, 0.0),
        mass: 1.0,
        radius: 1.0,
        luminosity: 0.0,
    };
    universe.bodies.add_body(body)?;
    let body_ref = universe.bodies.get_body(&name).expect("should work");
    *body_ref.mass = 2.0;
    let body_ref = universe.bodies.get_body(&name).expect("should work");
    assert!(*body_ref.mass == 2.0);
    universe.bodies.remove_body(&name).expect("should work");
    if !universe.bodies.get_body(&name).is_err() {
        panic!("should not have worked");
    }
    // *body_ref.mass = 1.0;

    let frametime_target = 1_000_000_000i64 / 60;
    'running: loop {
        let start = Instant::now();
        for event in window.event_pump.poll_iter() {
            match event {
                Event::Quit { .. } => break 'running,
                Event::KeyDown { .. } | Event::KeyUp { .. } => (),
                _ => {}
            }
        }
        let vertices = Vertices::new();
        window.render(&vertices);
        let end = Instant::now();
        let frame_time = (end - start).as_nanos() as i64;
        if frametime_target - frame_time >= 0 {
            spin_sleep::sleep(Duration::new(0, (frametime_target - frame_time) as u32));
        };
    }
    Ok(())
}
