use std::{error::Error, thread, time::Duration};

use sdl3::{event::Event, keyboard::Keycode, pixels::Color};

fn main() -> Result<(), Box<dyn Error>> {
    let sdl_context = sdl3::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();
    let window = video_subsystem
        .window("test window", 800, 600)
        .position_centered()
        .build()
        .unwrap();
    let mut canvas = window.into_canvas();
    let mut event_pump = sdl_context.event_pump().unwrap();
    let (mut r, mut g, mut b) = (0u8, 0u8, 0u8);
    let sleep_duration = Duration::new(0, 1_000_000_000u32 / 60);
    'running: loop {
        r = r.wrapping_add(3);
        g = g.wrapping_add(5);
        b = b.wrapping_add(7);
        canvas.set_draw_color(Color::RGB(r, g, b));
        canvas.clear();
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit { .. }
                | Event::KeyDown {
                    keycode: Some(Keycode::Escape),
                    ..
                } => break 'running,
                _ => {}
            }
        }
        canvas.present();
        thread::sleep(sleep_duration);
    }
    Ok(())
}
