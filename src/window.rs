use sdl3::EventPump;

use crate::{state::State, vertices::Vertices};

pub struct Window<'a> {
    pub event_pump: EventPump,
    state: State<'a>,
}

impl<'a> Window<'a> {
    pub fn new(size: (u32, u32)) -> Window<'a> {
        let sdl_context = sdl3::init().unwrap();
        let video_subsystem = sdl_context.video().unwrap();
        let window = video_subsystem
            .window("gravity-simulator", size.0, size.1)
            .position_centered()
            .metal_view()
            .build()
            .unwrap();
        let state = pollster::block_on(State::new(window, size));
        let event_pump = sdl_context.event_pump().unwrap();
        Window { event_pump, state }
    }

    pub fn render(&mut self, vertices: &Vertices) {
        self.state.render(vertices);
    }
}
