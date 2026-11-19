mod bodies;
mod state;
mod universe;
mod vertices;
mod window;

use std::{error::Error, thread, time::Duration};

use sdl3::{event::Event, keyboard::Keycode};

use crate::{vertices::Vertices, window::Window};

fn main() -> Result<(), Box<dyn Error>> {
    env_logger::init();
    let window_size = (800u32, 600u32);
    let mut window = Window::new(window_size);
    let vertices = Vertices::new();
    'running: loop {
        for event in window.event_pump.poll_iter() {
            match event {
                Event::Quit { .. }
                | Event::KeyDown {
                    keycode: Some(Keycode::Escape),
                    ..
                } => break 'running,
                _ => {}
            }
        }
        window.render(&vertices);
        thread::sleep(Duration::from_nanos(1_000_000_000u64 / 60));
    }
    Ok(())
}
