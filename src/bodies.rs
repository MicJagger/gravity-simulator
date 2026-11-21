use std::{collections::BTreeMap, error::Error, fs::File, io::BufReader};

use ordered_hash_map::OrderedHashMap;
use serde::Deserialize;
use serde_json::Deserializer;

#[derive(Deserialize)]
pub struct Body {
    pub name: String,
    /// x, y, z in meters
    pub position: (f64, f64, f64),
    /// x, y, z in meters/second
    pub velocity: (f64, f64, f64),
    /// theta, phi, psi in degrees
    pub rotation: (f32, f32, f32),
    /// theta, phi, psi in degrees/second
    pub rotation_velocity: (f32, f32, f32),
    /// in kilograms
    pub mass: f64,
    /// in meters
    pub radius: f64,
    /// in watts
    pub luminosity: f64,
}

#[allow(dead_code)]
pub struct BodyRef<'a> {
    name: &'a String,
    /// x, y, z in meters
    pub position: (&'a mut f64, &'a mut f64, &'a mut f64),
    /// x, y, z in meters
    pub velocity: (&'a mut f64, &'a mut f64, &'a mut f64),
    /// theta, phi, psi in degrees
    pub rotation: (&'a mut f32, &'a mut f32, &'a mut f32),
    /// theta, phi, psi in degrees
    pub rotation_velocity: (&'a mut f32, &'a mut f32, &'a mut f32),
    /// in kilograms
    pub mass: &'a mut f64,
    /// in meters
    pub radius: &'a mut f64,
    /// in watts
    pub luminosity: &'a mut f64,
}

#[allow(dead_code)]
impl BodyRef<'_> {
    pub fn name(&self) -> &String {
        self.name
    }
}

#[derive(Default, Debug)]
pub struct Bodies {
    indices_insertion_order: OrderedHashMap<String, usize>,
    indices_sorted: BTreeMap<String, usize>,
    names: Vec<String>,
    positions_x: Vec<f64>,
    positions_y: Vec<f64>,
    positions_z: Vec<f64>,
    vels_x: Vec<f64>,
    vels_y: Vec<f64>,
    vels_z: Vec<f64>,
    rots_theta: Vec<f32>,
    rots_phi: Vec<f32>,
    rots_psi: Vec<f32>,
    rot_vels_theta: Vec<f32>,
    rot_vels_phi: Vec<f32>,
    rot_vels_psi: Vec<f32>,
    masses: Vec<f64>,
    radii: Vec<f64>,
    luminosities: Vec<f64>,
}

impl Bodies {
    pub fn new() -> Self {
        Bodies::default()
    }

    #[allow(dead_code)]
    pub fn bodies_unsorted<'a>(&'a mut self) -> impl Iterator<Item = BodyRef<'a>> {
        itertools::izip!(
            &self.names,
            &mut self.positions_x,
            &mut self.positions_y,
            &mut self.positions_z,
            &mut self.vels_x,
            &mut self.vels_y,
            &mut self.vels_z,
            &mut self.rots_theta,
            &mut self.rots_phi,
            &mut self.rots_psi,
            &mut self.rot_vels_theta,
            &mut self.rot_vels_phi,
            &mut self.rot_vels_psi,
            &mut self.masses,
            &mut self.radii,
            &mut self.luminosities,
        )
        .map(|fields| BodyRef {
            name: fields.0,
            position: (fields.1, fields.2, fields.3),
            velocity: (fields.4, fields.5, fields.6),
            rotation: (fields.7, fields.8, fields.9),
            rotation_velocity: (fields.10, fields.11, fields.12),
            mass: fields.13,
            radius: fields.14,
            luminosity: fields.15,
        })
    }

    #[allow(dead_code)]
    pub fn get_body(&mut self, name: &str) -> Result<BodyRef<'_>, Box<dyn Error>> {
        let Some(index) = self.indices_insertion_order.get(name) else {
            return Err(format!("body by name {} not found", name).into());
        };
        Ok(self.get_body_by_index(*index))
    }

    pub fn get_body_by_index(&mut self, index: usize) -> BodyRef<'_> {
        BodyRef {
            name: &self.names[index],
            position: (
                &mut self.positions_x[index],
                &mut self.positions_y[index],
                &mut self.positions_z[index],
            ),
            velocity: (
                &mut self.vels_x[index],
                &mut self.vels_y[index],
                &mut self.vels_z[index],
            ),
            rotation: (
                &mut self.rots_theta[index],
                &mut self.rots_phi[index],
                &mut self.rots_psi[index],
            ),
            rotation_velocity: (
                &mut self.rot_vels_theta[index],
                &mut self.rot_vels_phi[index],
                &mut self.rot_vels_psi[index],
            ),
            mass: &mut self.masses[index],
            radius: &mut self.radii[index],
            luminosity: &mut self.luminosities[index],
        }
    }

    pub fn load_bodies(&mut self, file_path: &str) -> Result<(), Box<dyn Error>> {
        let file = File::open(file_path)?;
        let reader = BufReader::new(file);
        let stream = Deserializer::from_reader(reader).into_iter::<Body>();
        for body in stream {
            self.add_body(body?)?;
        }
        Ok(())
    }

    #[allow(dead_code)]
    pub fn clear_bodies(&mut self) {
        self.indices_insertion_order.clear();
        self.indices_sorted.clear();
        self.names.clear();
        self.positions_x.clear();
        self.positions_y.clear();
        self.positions_z.clear();
        self.vels_x.clear();
        self.vels_y.clear();
        self.vels_z.clear();
        self.rots_theta.clear();
        self.rots_phi.clear();
        self.rots_psi.clear();
        self.rot_vels_theta.clear();
        self.rot_vels_phi.clear();
        self.rot_vels_psi.clear();
        self.masses.clear();
        self.radii.clear();
        self.luminosities.clear();
    }

    pub fn add_body(&mut self, body: Body) -> Result<(), Box<dyn Error>> {
        if self.indices_insertion_order.contains_key(&body.name) {
            return Err(format!("body by name {} already in universe", body.name).into());
        };
        self.indices_insertion_order
            .insert(body.name.clone(), self.positions_x.len());
        self.indices_sorted
            .insert(body.name.clone(), self.positions_x.len());
        self.names.push(body.name);
        self.positions_x.push(body.position.0);
        self.positions_y.push(body.position.1);
        self.positions_z.push(body.position.2);
        self.vels_x.push(body.velocity.0);
        self.vels_y.push(body.velocity.1);
        self.vels_z.push(body.velocity.2);
        self.rots_theta.push(body.rotation.0);
        self.rots_phi.push(body.rotation.1);
        self.rots_psi.push(body.rotation.2);
        self.rot_vels_theta.push(body.rotation_velocity.0);
        self.rot_vels_phi.push(body.rotation_velocity.1);
        self.rot_vels_psi.push(body.rotation_velocity.2);
        self.masses.push(body.mass);
        self.radii.push(body.radius);
        self.luminosities.push(body.luminosity);
        Ok(())
    }

    #[allow(dead_code)]
    pub fn remove_body(&mut self, name: &str) -> Result<(), Box<dyn Error>> {
        let Some(index) = self.indices_insertion_order.get(name) else {
            return Err(format!("body by name {} not found", name).into());
        };
        let index: usize = *index;
        self.indices_insertion_order.remove(name);
        self.indices_sorted.remove(name);
        if index == self.positions_x.len() - 1 {
            self.names.pop();
            self.positions_x.pop();
            self.positions_y.pop();
            self.positions_z.pop();
            self.vels_x.pop();
            self.vels_y.pop();
            self.vels_z.pop();
            self.rots_theta.pop();
            self.rots_phi.pop();
            self.rots_psi.pop();
            self.rot_vels_theta.pop();
            self.rot_vels_phi.pop();
            self.rot_vels_psi.pop();
            self.masses.pop();
            self.radii.pop();
            self.luminosities.pop();
            return Ok(());
        }
        const UNRCHBL: &str = "this should be unreachable";
        self.names[index] = self.names.pop().expect(UNRCHBL);
        self.positions_x[index] = self.positions_x.pop().expect(UNRCHBL);
        self.positions_y[index] = self.positions_y.pop().expect(UNRCHBL);
        self.positions_z[index] = self.positions_z.pop().expect(UNRCHBL);
        self.vels_x[index] = self.vels_x.pop().expect(UNRCHBL);
        self.vels_y[index] = self.vels_y.pop().expect(UNRCHBL);
        self.vels_z[index] = self.vels_z.pop().expect(UNRCHBL);
        self.rots_theta[index] = self.rots_theta.pop().expect(UNRCHBL);
        self.rots_phi[index] = self.rots_phi.pop().expect(UNRCHBL);
        self.rots_psi[index] = self.rots_psi.pop().expect(UNRCHBL);
        self.rot_vels_theta[index] = self.rot_vels_theta.pop().expect(UNRCHBL);
        self.rot_vels_phi[index] = self.rot_vels_phi.pop().expect(UNRCHBL);
        self.rot_vels_psi[index] = self.rot_vels_psi.pop().expect(UNRCHBL);
        self.masses[index] = self.masses.pop().expect(UNRCHBL);
        self.radii[index] = self.radii.pop().expect(UNRCHBL);
        self.luminosities[index] = self.luminosities.pop().expect(UNRCHBL);
        Ok(())
    }
}
